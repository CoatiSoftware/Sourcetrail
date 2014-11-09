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
	GraphEdge(const Edge* data);
	virtual ~GraphEdge();

	virtual void ownerMoved() = 0;
	virtual void targetMoved() = 0;

	virtual void removeEdgeFromScene() = 0;

	virtual std::weak_ptr<GraphNode> getOwner() = 0;
	virtual std::weak_ptr<GraphNode> getTarget() = 0;

	Id getTokenId() const;
	const Edge* getData() const;

protected:
	const Edge* m_data;
};

// temporary data structure for (visual) graph creation process
struct DummyEdge
{
	DummyEdge(const Id o, const Id t, const Edge* data)
		: ownerId(o)
		, targetId(t)
		, data(data)
	{
	}

	Id ownerId;
	Id targetId;

	const Edge* data;
};

#endif // GRAPH_EDGE_H
