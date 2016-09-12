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

bool Blackboard::exists(const std::string& key)
{
	ItemMap::const_iterator it = m_values.find(key);
	return (it != m_values.end());
}

bool Blackboard::clear(const std::string& key)
{
	ItemMap::const_iterator it = m_values.find(key);
	if (it != m_values.end())
	{
		m_values.erase(it);
		return true;
	}
	return false;
}
