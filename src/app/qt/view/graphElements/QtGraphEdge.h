#ifndef QT_GRAPH_EDGE_H
#define QT_GRAPH_EDGE_H

#include <memory>

#include <QGraphicsItem>

#include "utility/math/Vector2.h"

#include "component/view/graphElements/GraphEdge.h"

class GraphNode;
class QtGraphicsRoundedRectItem;

class QtStraightConnection
	: public QGraphicsLineItem
{
public:
	QtStraightConnection(Vec4i ownerRect, Vec4i targetRect, int number, QGraphicsItem* parent);
	virtual ~QtStraightConnection();

	void setColor(QColor color);

private:
	QtGraphicsRoundedRectItem* m_circle;
	QGraphicsSimpleTextItem* m_number;
};

class QtCorneredConnection
	: public QGraphicsLineItem
{
public:
	QtCorneredConnection(Vec4i ownerRect, Vec4i targetRect, Vec4i ownerParentRect, Vec4i targetParentRect, QGraphicsItem* parent);
	virtual ~QtCorneredConnection();

	virtual QPainterPath shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem* options, QWidget* widget);

	void setClosed(bool closed);
	void setBig(bool big);

private:
	QPolygon getPath() const;
	int getDirection(const QPointF& a, const QPointF& b) const;

	Vec4i m_ownerRect;
	Vec4i m_targetRect;

	Vec4i m_ownerParentRect;
	Vec4i m_targetParentRect;

	bool m_closed;
	bool m_big;
};


class QtGraphEdge
	: public GraphEdge
	, public QGraphicsItemGroup
{
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
	bool isAggregation() const;
	bool isInheritance() const;

	int getZValue(bool active) const;
	float getPenWidth() const;
	int getAggregationCount() const;

	std::weak_ptr<GraphNode> m_owner;
	std::weak_ptr<GraphNode> m_target;

	QGraphicsLineItem* m_child;

	bool m_isActive;

	Vec2i m_mousePos;
	bool m_mouseMoved;
};

#endif // QT_GRAPH_EDGE_H
