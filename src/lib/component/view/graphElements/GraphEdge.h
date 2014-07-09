#ifndef GRAPH_EDGE_H
#define GRAPH_EDGE_H

#include <memory>

#include "utility/types.h"

class GraphNode;

class GraphEdge
{
public:
	GraphEdge();
	virtual ~GraphEdge();

	virtual void ownerMoved() = 0;
	virtual void targetMoved() = 0;

	virtual void removeEdgeFromScene() = 0;

	virtual std::weak_ptr<GraphNode> getOwner() = 0;
	virtual std::weak_ptr<GraphNode> getTarget() = 0;
};

// temporary data structure for (visual) graph creation process
struct DummyEdge
{
public:
	DummyEdge(const Id o, const Id t)
		: ownerId(o)
		, targetId(t)
	{
	}

	Id ownerId;
	Id targetId;

	bool operator==(const DummyEdge& other) const
	{
		if(ownerId == other.ownerId && targetId == other.targetId)
		{
			return true;
		}
		return false;
	}

	bool operator!=(const DummyEdge& other) const
	{
		return !(*this == other);
	}
};

#endif // GRAPH_EDGE_H