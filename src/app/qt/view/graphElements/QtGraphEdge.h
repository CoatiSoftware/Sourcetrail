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

	virtual void removeEdgeFromScene();

	virtual std::weak_ptr<GraphNode> getOwner();
	virtual std::weak_ptr<GraphNode> getTarget();

	virtual void setColor(const Vec4i& color);
	virtual Vec4i getColor() const;

private:
	std::weak_ptr<GraphNode> m_owner;
	std::weak_ptr<GraphNode> m_target;

	Vec4i m_color;
};

#endif // QT_GRAPH_EDGE_H
