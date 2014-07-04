#ifndef GRAPH_NODE_H
#define GRAPH_NODE_H

#include "utility/math/Vector2.h"

class GraphEdge;

class GraphNode
{
public:
	GraphNode();
	virtual ~GraphNode();

	virtual Vec2i getPosition() = 0;

	virtual void addOutEdge(const std::shared_ptr<GraphEdge>& edge) = 0;
	virtual void addInEdge(const std::weak_ptr<GraphEdge>& edge) = 0;

	virtual void removeOutEdge(const std::shared_ptr<GraphEdge>& edge) = 0;
};

#endif // GRAPH_NODE_H
