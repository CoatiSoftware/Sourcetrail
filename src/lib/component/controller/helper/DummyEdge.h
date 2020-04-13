#ifndef DUMMY_EDGE_H
#define DUMMY_EDGE_H

#include "../../../utility/math/Vector4.h"
#include "../../../utility/types.h"

#include "../../../data/graph/Edge.h"
#include "../../../data/graph/token_component/TokenComponentAggregation.h"

class Edge;

// temporary data structure for (visual) graph creation process
struct DummyEdge
{
	DummyEdge()
		: ownerId(0)
		, targetId(0)
		, data(nullptr)
		, visible(false)
		, hidden(false)
		, active(false)
		, layoutHorizontal(true)
		, weight(0)
		, direction(TokenComponentAggregation::DIRECTION_INVALID)
	{
	}

	DummyEdge(const Id ownerId, const Id targetId, const Edge* data)
		: ownerId(ownerId)
		, targetId(targetId)
		, data(data)
		, visible(false)
		, hidden(false)
		, active(false)
		, layoutHorizontal(true)
		, weight(0)
		, direction(TokenComponentAggregation::DIRECTION_INVALID)
	{
	}

	int getWeight() const
	{
		if (!data)
		{
			return weight;
		}
		else if (data->isType(Edge::EDGE_AGGREGATION))
		{
			return data->getComponent<TokenComponentAggregation>()->getAggregationCount();
		}

		return 1;
	}

	void updateDirection(TokenComponentAggregation::Direction dir, bool invert)
	{
		if (invert)
		{
			dir = TokenComponentAggregation::opposite(dir);
		}

		if (direction == TokenComponentAggregation::DIRECTION_INVALID)
		{
			direction = dir;
		}
		else if (direction != dir)
		{
			direction = TokenComponentAggregation::DIRECTION_NONE;
		}
	}

	TokenComponentAggregation::Direction getDirection() const
	{
		if (!data)
		{
			return direction;
		}
		else if (data->isType(Edge::EDGE_AGGREGATION))
		{
			return data->getComponent<TokenComponentAggregation>()->getDirection();
		}

		return TokenComponentAggregation::DIRECTION_FORWARD;
	}

	Id ownerId;
	Id targetId;

	const Edge* data;

	bool visible;
	bool hidden;
	bool active;

	std::vector<Vec4i> path;

	bool layoutHorizontal;

	// BundleEdge
	int weight;
	TokenComponentAggregation::Direction direction;
};

#endif	  // DUMMY_EDGE_H
