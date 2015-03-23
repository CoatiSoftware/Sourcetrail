#ifndef QT_GRAPH_EDGE_H
#define QT_GRAPH_EDGE_H

#include <memory>

#include <QGraphicsItem>

#include "utility/math/Vector2.h"

#include "component/view/graphElements/GraphEdge.h"

class GraphNode;

class QtGraphEdge
	: public QObject
	, public GraphEdge
	, public QGraphicsItemGroup
{
	Q_OBJECT
	Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
	QtGraphEdge(const std::weak_ptr<GraphNode>& owner, const std::weak_ptr<GraphNode>& target, const Edge* data);
	virtual ~QtGraphEdge();

	virtual std::weak_ptr<GraphNode> getOwner();
	virtual std::weak_ptr<GraphNode> getTarget();

	virtual void updateLine();

	bool getIsActive() const;
	void setIsActive(bool isActive);

	void onClick();

protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

private:
	std::weak_ptr<GraphNode> m_owner;
	std::weak_ptr<GraphNode> m_target;

	QGraphicsLineItem* m_child;

	bool m_isActive;

	Vec2i m_mousePos;
	bool m_mouseMoved;
};

#endif // QT_GRAPH_EDGE_H
