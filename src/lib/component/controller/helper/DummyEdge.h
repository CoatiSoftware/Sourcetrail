#ifndef DUMMY_EDGE_H
#define DUMMY_EDGE_H

#include "utility/types.h"

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

	Id ownerId;
	Id targetId;

	const Edge* data;

	bool visible;
	bool active;
};

#endif // DUMMY_EDGE_H
