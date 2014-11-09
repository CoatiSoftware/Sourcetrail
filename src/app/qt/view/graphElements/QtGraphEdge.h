#ifndef QT_GRAPH_EDGE_H
#define QT_GRAPH_EDGE_H

#include <memory>

#include <QGraphicsItem>

#include "utility/math/Vector2.h"
#include "utility/messaging/type/MessageActivateToken.h"

#include "component/view/graphElements/GraphEdge.h"

class GraphNode;

class QtGraphEdge
	: public GraphEdge
	, public QGraphicsLineItem
{
public:
	QtGraphEdge(const std::weak_ptr<GraphNode>& owner, const std::weak_ptr<GraphNode>& target, const Edge* data);
	virtual ~QtGraphEdge();

	virtual void ownerMoved();
	virtual void targetMoved();

	virtual void removeEdgeFromScene();

	virtual std::weak_ptr<GraphNode> getOwner();
	virtual std::weak_ptr<GraphNode> getTarget();

	bool getIsActive() const;
	void setIsActive(bool isActive);

	void onClick();

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

	void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

private:
	std::weak_ptr<GraphNode> m_owner;
	std::weak_ptr<GraphNode> m_target;

	bool m_isActive;

	QGraphicsLineItem* m_child;

	Vec2i m_mousePos;
	bool m_mouseMoved;
};

#endif // QT_GRAPH_EDGE_H
