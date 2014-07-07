#ifndef GRAPH_EDGE_H
#define GRAPH_EDGE_H

#include <memory>

class GraphNode;

class GraphEdge
{
public:
	GraphEdge();
	virtual ~GraphEdge();

	virtual void ownerMoved() = 0;
	virtual void targetMoved() = 0;
};

// temporary data structure for (visual) graph creation process
struct DummyEdge
{
public:
	DummyEdge(const std::weak_ptr<GraphNode> o, const std::weak_ptr<GraphNode> t)
		: owner(o)
		, target(t)
	{
	}

	std::weak_ptr<GraphNode> owner;
	std::weak_ptr<GraphNode> target;
};

#endif // GRAPH_EDGE_H