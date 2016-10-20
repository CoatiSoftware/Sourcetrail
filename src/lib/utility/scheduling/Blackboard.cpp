#include "utility/scheduling/Blackboard.h"

Blackboard::Blackboard()
{
}

Blackboard::Blackboard(std::shared_ptr<Blackboard> parent)
	: m_parent(parent)
{
}

Blackboard::~Blackboard()
{
}

std::mutex& Blackboard::getMutex()
{
	return m_mutex;
}

bool Blackboard::exists(const std::string& key)
{
	std::lock_guard<std::mutex> lock(m_itemMutex);

	ItemMap::const_iterator it = m_items.find(key);
	return (it != m_items.end());
}

bool Blackboard::clear(const std::string& key)
{
	std::lock_guard<std::mutex> lock(m_itemMutex);

	ItemMap::const_iterator it = m_items.find(key);
	if (it != m_items.end())
	{
		m_items.erase(it);
		return true;
	}
	return false;
}
