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

	bool operator<(const DummyEdge& other) const
	{
		if(ownerId < other.ownerId )
		{
			return true;
		}
		else if(ownerId == other.ownerId)
		{
			return (targetId < other.targetId);
		}
		return false;
	}

	bool operator>(const DummyEdge& other) const
	{
		return !(*this < other);
	}

	Id ownerId;
	Id targetId;
};

#endif // GRAPH_EDGE_H