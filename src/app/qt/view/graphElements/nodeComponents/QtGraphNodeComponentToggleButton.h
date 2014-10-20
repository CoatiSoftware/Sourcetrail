#ifndef QT_GRAPH_NODE_COMPONENT_TOGGLE_BUTTON_H
#define QT_GRAPH_NODE_COMPONENT_TOGGLE_BUTTON_H

#include "QtGraphNodeComponent.h"

class QtGraphNodeComponentToggleButton
	: public QtGraphNodeComponent
	, public QGraphicsRectItem
{
public:
	QtGraphNodeComponentToggleButton(const std::weak_ptr<QtGraphNode>& graphNode);
	~QtGraphNodeComponentToggleButton();

	virtual void nodeMousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void nodeMouseMoveEvent(QGraphicsSceneMouseEvent* event);
	virtual void nodeMouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

	void mousePressEvent(QGraphicsSceneMouseEvent* event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
};

#endif // QT_GRAPH_NODE_COMPONENT_TOGGLE_BUTTON_H
