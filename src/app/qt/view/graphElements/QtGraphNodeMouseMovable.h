#ifndef QT_GRAPH_NODE_MOUSE_MOVABLE_H
#define QT_GRAPH_NODE_MOUSE_MOVABLE_H

#include "qt/view/graphElements/QtGraphNode.h"

class QtGraphNodeMouseMovable : public QtGraphNode
{
public:
	QtGraphNodeMouseMovable(const Vec2i& position, const std::string& name, const Id tokenId);
	virtual ~QtGraphNodeMouseMovable();

	void mousePressEvent(QGraphicsSceneMouseEvent* event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

private:
	Vec2i m_mouseOffset;
};

#endif // QT_GRAPH_NODE_MOUSE_MOVABLE_H