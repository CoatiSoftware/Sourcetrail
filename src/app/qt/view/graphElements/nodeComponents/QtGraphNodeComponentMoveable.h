#ifndef QT_GRAPH_NODE_COMPONENT_MOVEABLE
#define QT_GRAPH_NODE_COMPONENT_MOVEABLE

#include "QtGraphNodeComponent.h"

#include "utility/math/Vector2.h"

class QtGraphNodeComponentMoveable
	: public QtGraphNodeComponent
{
public:
	QtGraphNodeComponentMoveable(const std::weak_ptr<QtGraphNode>& graphNode);
	~QtGraphNodeComponentMoveable();

	virtual void nodeMousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void nodeMouseMoveEvent(QGraphicsSceneMouseEvent* event);
	virtual void nodeMouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

private:
	Vec2i m_mouseOffset;
};

#endif // QT_GRAPH_NODE_COMPONENT_MOVEABLE