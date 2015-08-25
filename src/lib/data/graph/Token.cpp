#include "data/graph/Token.h"

#include "data/location/TokenLocation.h"
#include "utility/logging/logging.h"

void Token::resetNextId()
{
	s_nextId = 1;
}

Token::Token()	// TODO: remove this constructor
	: m_id(s_nextId++)
{
}

Token::Token(Id id)
	: m_id(id)
{
}

Token::~Token()
{
}

Id Token::getId() const
{
	return m_id;
}

const std::vector<Id>& Token::getLocationIds() const
{
	return m_locationIds;
}

void Token::addLocationId(Id locationId)
{
	m_locationIds.push_back(locationId);
}

void Token::removeLocationId(Id locationId)
{
	for (std::vector<Id>::const_iterator it = m_locationIds.begin(); it != m_locationIds.end(); it++)
	{
		if (*it == locationId)
		{
			m_locationIds.erase(it);
			return;
		}
	}

	LOG_ERROR("Location Id was not referenced by this Token.");
}

Token::Token(const Token& other)
	: m_id(other.m_id)
{
	for (std::shared_ptr<TokenComponent> component: other.m_components)
	{
		addComponent(component->copy());
	}
}

void Token::addComponent(std::shared_ptr<TokenComponent> component)
{
	m_components.push_back(component);
}

Id Token::s_nextId = 1;
