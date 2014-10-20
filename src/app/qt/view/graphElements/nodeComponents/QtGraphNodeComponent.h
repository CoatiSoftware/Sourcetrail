#ifndef QT_GRAPH_NODE_COMPONENT_H
#define QT_GRAPH_NODE_COMPONENT_H

#include <memory>

#include <QGraphicsItem>

class QtGraphNode;

class QtGraphNodeComponent
{
public:
	QtGraphNodeComponent(const std::weak_ptr<QtGraphNode>& graphNode);
	~QtGraphNodeComponent();

	virtual void nodeMousePressEvent(QGraphicsSceneMouseEvent* event) = 0;
	virtual void nodeMouseMoveEvent(QGraphicsSceneMouseEvent* event) = 0;
	virtual void nodeMouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) = 0;

protected:
	std::weak_ptr<QtGraphNode> m_graphNode;
};

#endif // QT_GRAPH_NODE_COMPONENT_H
