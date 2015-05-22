#ifndef QT_GRAPH_EDGE_H
#define QT_GRAPH_EDGE_H

#include <memory>

#include <QGraphicsItem>

#include "utility/math/Vector2.h"

class Edge;
class QtGraphNode;

class QtGraphEdge
	: public QObject
	, public QGraphicsItemGroup
{
	Q_OBJECT
	Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
	QtGraphEdge(const std::weak_ptr<QtGraphNode>& owner, const std::weak_ptr<QtGraphNode>& target, const Edge* data);
	virtual ~QtGraphEdge();

	const Edge* getData() const;

	virtual std::weak_ptr<QtGraphNode> getOwner();
	virtual std::weak_ptr<QtGraphNode> getTarget();

	virtual void updateLine();

	bool getIsActive() const;
	void setIsActive(bool isActive);

	void onClick();

	void focusIn();
	void focusOut();

protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

private:
	const Edge* m_data;

	std::weak_ptr<QtGraphNode> m_owner;
	std::weak_ptr<QtGraphNode> m_target;

	QGraphicsLineItem* m_child;

	bool m_isActive;

	Vec2i m_mousePos;
	bool m_mouseMoved;
};

#endif // QT_GRAPH_EDGE_H
