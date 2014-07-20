#ifndef GRAPH_EDGE_H
#define GRAPH_EDGE_H

#include <memory>

#include "data/graph/Edge.h"
#include "utility/math/Vector4.h"
#include "utility/types.h"

class GraphNode;

class GraphEdge
{
public:
	GraphEdge(const Id tokenId);
	virtual ~GraphEdge();

	virtual void ownerMoved() = 0;
	virtual void targetMoved() = 0;

	virtual void removeEdgeFromScene() = 0;

	virtual std::weak_ptr<GraphNode> getOwner() = 0;
	virtual std::weak_ptr<GraphNode> getTarget() = 0;

	Id getTokenId() const;

	virtual void setColor(const Vec4i& color) = 0;
	virtual Vec4i getColor() const = 0;

protected:
	Id m_tokenId;
};

// temporary data structure for (visual) graph creation process
struct DummyEdge
{
public:
	DummyEdge(const Id o, const Id t, const Id tknId, Edge::EdgeType type)
		: ownerId(o)
		, targetId(t)
		, tokenId(tknId)
		, edgeType(type)
	{
	}

	bool operator==(const DummyEdge& other) const
	{
		if (ownerId == other.ownerId && targetId == other.targetId)
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
		if (ownerId < other.ownerId )
		{
			return true;
		}
		else if (ownerId == other.ownerId)
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
	Id tokenId;

	Edge::EdgeType edgeType;
};

#endif // GRAPH_EDGE_H
