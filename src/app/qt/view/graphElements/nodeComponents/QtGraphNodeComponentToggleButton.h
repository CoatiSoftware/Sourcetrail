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

	void mousePressEvent(QGraphicsSceneMouseEvent* event);
};

#endif // QT_GRAPH_NODE_COMPONENT_TOGGLE_BUTTON_H
