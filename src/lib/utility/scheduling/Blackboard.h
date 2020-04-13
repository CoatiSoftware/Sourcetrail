#ifndef BLACKBOARD_H
#define BLACKBOARD_H

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "../logging/logging.h"

struct BlackboardItemBase
{
	virtual ~BlackboardItemBase() {}
};

template <typename T>
struct BlackboardItem: public BlackboardItemBase
{
	BlackboardItem(const T& v): value(v) {}

	virtual ~BlackboardItem() {}

	T value;
};

class Blackboard
{
public:
	Blackboard();
	Blackboard(std::shared_ptr<Blackboard> parent);

	// atomic way to set a blackboard value
	template <typename T>
	void set(const std::string& key, const T& value);

	// atomic way to get a blackboard value
	template <typename T>
	bool get(const std::string& key, T& value);

	// atomic way to update a blackboard value
	template <typename T>
	bool update(const std::string& key, std::function<T(const T&)> updater);

	bool exists(const std::string& key);
	bool clear(const std::string& key);

private:
	typedef std::map<std::string, std::shared_ptr<BlackboardItemBase>> ItemMap;

	std::shared_ptr<Blackboard> m_parent;

	ItemMap m_items;
	std::mutex m_itemMutex;
};


template <typename T>
void Blackboard::set(const std::string& key, const T& value)
{
	std::lock_guard<std::mutex> lock(m_itemMutex);

	m_items[key] = std::make_shared<BlackboardItem<T>>(value);
}

template <typename T>
bool Blackboard::get(const std::string& key, T& value)
{
	std::lock_guard<std::mutex> lock(m_itemMutex);

	ItemMap::const_iterator it = m_items.find(key);
	if (it != m_items.end())
	{
		if (std::shared_ptr<BlackboardItem<T>> item = std::dynamic_pointer_cast<BlackboardItem<T>>(
				it->second))
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

template <typename T>
bool Blackboard::update(const std::string& key, std::function<T(const T&)> updater)
{
	std::lock_guard<std::mutex> lock(m_itemMutex);

	ItemMap::const_iterator it = m_items.find(key);
	if (it != m_items.end())
	{
		if (std::shared_ptr<BlackboardItem<T>> item = std::dynamic_pointer_cast<BlackboardItem<T>>(
				it->second))
		{
			item->value = updater(item->value);
			return true;
		}
	}

	LOG_WARNING("Entry for \"" + key + "\" not found on blackboard.");
	return false;
}

#endif	  // BLACKBOARD_H
