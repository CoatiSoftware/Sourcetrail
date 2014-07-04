#ifndef QT_GRAPH_EDGE_H
#define QT_GRAPH_EDGE_H

#include <memory>

#include "qgraphicsitem.h"

#include "component/view/graphElements/GraphEdge.h"

class GraphNode;

class QtGraphEdge:
	public GraphEdge,
	public QGraphicsLineItem
{
public:
	QtGraphEdge(const std::weak_ptr<GraphNode>& owner, const std::weak_ptr<GraphNode>& target);
	virtual ~QtGraphEdge();

	virtual void ownerMoved();
	virtual void targetMoved();

private:
	std::weak_ptr<GraphNode> m_owner;
	std::weak_ptr<GraphNode> m_target;
};

#endif // QT_GRAPH_EDGE_H
