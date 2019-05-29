#ifndef QT_GRAPH_NODE_COMPONENT_CLICKABLE
#define QT_GRAPH_NODE_COMPONENT_CLICKABLE

#include "QtGraphNodeComponent.h"

#include "Vector2.h"

class QtGraphNodeComponentClickable
	: public QtGraphNodeComponent
{
public:
	QtGraphNodeComponentClickable(QtGraphNode* graphNode);
	virtual ~QtGraphNodeComponentClickable();

	virtual void nodeMousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void nodeMouseMoveEvent(QGraphicsSceneMouseEvent* event);
	virtual void nodeMouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:
	Vec2i m_mousePos;
	bool m_mouseMoved;
};

#endif // QT_GRAPH_NODE_COMPONENT_CLICKABLE
