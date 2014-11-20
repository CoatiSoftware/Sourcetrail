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

	virtual std::weak_ptr<GraphNode> getOwner() = 0;
	virtual std::weak_ptr<GraphNode> getTarget() = 0;

	virtual void updateLine() = 0;

	const Edge* getData() const;

private:
	const Edge* m_data;
};

// temporary data structure for (visual) graph creation process
struct DummyEdge
{
	DummyEdge(const Id o, const Id t, const Edge* data)
		: ownerId(o)
		, targetId(t)
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

#endif // GRAPH_EDGE_H
