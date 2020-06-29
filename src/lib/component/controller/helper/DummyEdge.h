#ifndef DUMMY_EDGE_H
#define DUMMY_EDGE_H

#include "Vector4.h"
#include "types.h"

#include "Edge.h"
#include "TokenComponentBundledEdges.h"

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
		, direction(TokenComponentBundledEdges::DIRECTION_INVALID)
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
		, direction(TokenComponentBundledEdges::DIRECTION_INVALID)
	{
	}

	int getWeight() const
	{
		if (!data)
		{
			return weight;
		}
		else if (data->isType(Edge::EDGE_BUNDLED_EDGES))
		{
			return data->getComponent<TokenComponentBundledEdges>()->getBundledEdgesCount();
		}

		return 1;
	}

	void updateDirection(TokenComponentBundledEdges::Direction dir, bool invert)
	{
		if (invert)
		{
			dir = TokenComponentBundledEdges::opposite(dir);
		}

		if (direction == TokenComponentBundledEdges::DIRECTION_INVALID)
		{
			direction = dir;
		}
		else if (direction != dir)
		{
			direction = TokenComponentBundledEdges::DIRECTION_NONE;
		}
	}

	TokenComponentBundledEdges::Direction getDirection() const
	{
		if (!data)
		{
			return direction;
		}
		else if (data->isType(Edge::EDGE_BUNDLED_EDGES))
		{
			return data->getComponent<TokenComponentBundledEdges>()->getDirection();
		}

		return TokenComponentBundledEdges::DIRECTION_FORWARD;
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
	TokenComponentBundledEdges::Direction direction;
};

#endif	  // DUMMY_EDGE_H
