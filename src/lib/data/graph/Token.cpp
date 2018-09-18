#include "Token.h"

#include "logging.h"

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

void Token::addComponent(std::shared_ptr<TokenComponent> component)
{
	m_components.push_back(component);
}

Token::Token(const Token& other)
	: m_id(other.m_id)
{
	for (const std::shared_ptr<TokenComponent>& component: other.m_components)
	{
		addComponent(component->copy());
	}
}
