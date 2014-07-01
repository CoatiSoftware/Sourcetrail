#include "data/graph/Token.h"

#include "data/location/TokenLocation.h"
#include "utility/logging/logging.h"

Token::Token()
	: m_id(s_nextId++)
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

Token::Token(Id id)
	: m_id(id)
{
}

Id Token::s_nextId = 1;
