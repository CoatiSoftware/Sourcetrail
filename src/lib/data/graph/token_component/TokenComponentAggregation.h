#ifndef TOKEN_COMPONENT_AGGREGATION_H
#define TOKEN_COMPONENT_AGGREGATION_H

#include <map>
#include <set>

#include "types.h"

#include "TokenComponent.h"

class TokenComponentAggregation: public TokenComponent
{
public:
	enum Direction
	{
		DIRECTION_NONE,
		DIRECTION_FORWARD,
		DIRECTION_BACKWARD,
		DIRECTION_INVALID
	};

	static Direction opposite(Direction direction);

	TokenComponentAggregation();
	virtual ~TokenComponentAggregation();

	virtual std::shared_ptr<TokenComponent> copy() const;

	int getAggregationCount() const;
	std::set<Id> getAggregationIds() const;

	void addAggregationId(Id id, bool forward);
	void removeAggregationId(Id id);

	Direction getDirection();

private:
	std::map<Id, Direction> m_ids;
	Direction m_direction;
};

#endif	  // TOKEN_COMPONENT_AGGREGATION_H
