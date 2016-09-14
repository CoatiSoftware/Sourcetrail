#ifndef BLACKBOARD_H
#define BLACKBOARD_H

#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "utility/logging/logging.h"

struct BlackboardItemBase
{
	virtual ~BlackboardItemBase()
	{
	}
};

template <typename T>
struct BlackboardItem: public BlackboardItemBase
{
	BlackboardItem(const T &v)
		: value(v)
	{
	}

	virtual ~BlackboardItem()
	{
	}

	T value;
};

class Blackboard
{
public:
	Blackboard();
	Blackboard(std::shared_ptr<Blackboard> parent);
	~Blackboard();

	template <typename T>
	void set(const std::string& key, const T& value);

	template <typename T>
	bool get(const std::string& key, T& value);

	bool exists(const std::string& key);
	bool clear(const std::string& key);

private:
	typedef std::map<std::string, std::shared_ptr<BlackboardItemBase>> ItemMap;
	std::mutex m_mutex;

	std::shared_ptr<Blackboard> m_parent;
	ItemMap m_values;
};


template <typename T>
void Blackboard::set(const std::string& key, const T& value)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_values[key] = std::make_shared<BlackboardItem<T>>(value);
}

template <typename T>
bool Blackboard::get(const std::string& key, T& value)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	ItemMap::const_iterator it = m_values.find(key);
	if (it != m_values.end())
	{
		std::shared_ptr<BlackboardItem<T>> item = std::dynamic_pointer_cast<BlackboardItem<T>>(it->second);
		if (item)
		{
			value = item->value;
			return true;
		}
	}
	if (m_parent)
	{
		return m_parent->get(key, value);
	}

	LOG_WARNING("Entry for \"" + key + "\" not found on blackboard.");
	return false;
}

#endif // BLACKBOARD_H
