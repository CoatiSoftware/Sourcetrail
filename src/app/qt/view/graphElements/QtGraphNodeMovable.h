#ifndef QT_GRAPH_NODE_MOVABLE_H
#define QT_GRAPH_NODE_MOVABLE_H

#include "qt/view/graphElements/QtGraphNode.h"

class QtGraphNodeMovable : public QtGraphNode
{
public:
	QtGraphNodeMovable(const Vec2i& position, const std::string& name, const Id tokenId);
	virtual ~QtGraphNodeMovable();

	void mousePressEvent(QGraphicsSceneMouseEvent* event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

private:
	Vec2i m_mouseOffset;
};

#endif // QT_GRAPH_NODE_MOVABLE_H