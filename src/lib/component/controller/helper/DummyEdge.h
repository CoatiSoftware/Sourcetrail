#ifndef DUMMY_EDGE_H
#define DUMMY_EDGE_H

#include "utility/types.h"

#include "data/graph/Edge.h"
#include "data/graph/token_component/TokenComponentAggregation.h"

class Edge;

// temporary data structure for (visual) graph creation process
struct DummyEdge
{
	DummyEdge(const Id ownerId, const Id targetId, const Edge* data)
		: ownerId(ownerId)
		, targetId(targetId)
		, data(data)
		, visible(false)
		, active(false)
	{
	}

	int getWeight() const
	{
		if (data->isType(Edge::EDGE_AGGREGATION))
		{
			return data->getComponent<TokenComponentAggregation>()->getAggregationCount();
		}

		return 1;
	}

	Id ownerId;
	Id targetId;

	const Edge* data;

	bool visible;
	bool active;
};

#endif // DUMMY_EDGE_H
