#ifndef QT_GRAPH_NODE_COMPONENT_H
#define QT_GRAPH_NODE_COMPONENT_H

#include <memory>

#include <QGraphicsItem>

class QtGraphNode;

class QtGraphNodeComponent
{
public:
	QtGraphNodeComponent(const std::weak_ptr<QtGraphNode>& graphNode);
	virtual ~QtGraphNodeComponent();

	virtual void nodeMousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void nodeMouseMoveEvent(QGraphicsSceneMouseEvent* event);
	virtual void nodeMouseReleaseEvent(QGraphicsSceneMouseEvent* event);

protected:
	std::weak_ptr<QtGraphNode> m_graphNode;
};

#endif // QT_GRAPH_NODE_COMPONENT_H
