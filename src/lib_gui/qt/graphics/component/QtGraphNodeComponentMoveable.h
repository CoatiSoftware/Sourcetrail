#ifndef QT_GRAPH_NODE_COMPONENT_MOVEABLE
#define QT_GRAPH_NODE_COMPONENT_MOVEABLE

#include "QtGraphNodeComponent.h"

#include "../../../../lib/utility/math/Vector2.h"

class QtGraphNodeComponentMoveable: public QtGraphNodeComponent
{
public:
	QtGraphNodeComponentMoveable(QtGraphNode* graphNode);
	virtual ~QtGraphNodeComponentMoveable();

	virtual void nodeMousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void nodeMouseMoveEvent(QGraphicsSceneMouseEvent* event);
	virtual void nodeMouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:
	Vec2i m_mouseOffset;
	Vec2i m_oldPos;
};

#endif	  // QT_GRAPH_NODE_COMPONENT_MOVEABLE
