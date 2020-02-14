#include "TokenComponentAggregation.h"

TokenComponentAggregation::Direction TokenComponentAggregation::opposite(Direction direction)
{
	if (direction == DIRECTION_FORWARD)
	{
		return DIRECTION_BACKWARD;
	}
	else if (direction == DIRECTION_BACKWARD)
	{
		return DIRECTION_FORWARD;
	}

	return direction;
}

TokenComponentAggregation::TokenComponentAggregation(): m_direction(DIRECTION_INVALID) {}

TokenComponentAggregation::~TokenComponentAggregation() {}

std::shared_ptr<TokenComponent> TokenComponentAggregation::copy() const
{
	return std::make_shared<TokenComponentAggregation>(*this);
}

int TokenComponentAggregation::getAggregationCount() const
{
	return static_cast<int>(m_ids.size());
}

std::set<Id> TokenComponentAggregation::getAggregationIds() const
{
	std::set<Id> ids;

	for (const std::pair<Id, Direction>& p: m_ids)
	{
		ids.insert(p.first);
	}

	return ids;
}

void TokenComponentAggregation::addAggregationId(Id id, bool forward)
{
	m_ids.emplace(id, forward ? DIRECTION_FORWARD : DIRECTION_BACKWARD);

	m_direction = DIRECTION_INVALID;
}

void TokenComponentAggregation::removeAggregationId(Id id)
{
	m_ids.erase(id);

	m_direction = DIRECTION_INVALID;
}

TokenComponentAggregation::Direction TokenComponentAggregation::getDirection()
{
	if (m_direction != DIRECTION_INVALID)
	{
		return m_direction;
	}

	m_direction = DIRECTION_NONE;

	for (const std::pair<Id, Direction>& p: m_ids)
	{
		if (m_direction == DIRECTION_NONE)
		{
			m_direction = p.second;
		}
		else if (m_direction != p.second)
		{
			m_direction = DIRECTION_NONE;
			break;
		}
	}

	return m_direction;
}
