#ifndef QT_GRAPH_EDGE_H
#define QT_GRAPH_EDGE_H

#include <memory>

#include <QGraphicsItem>

#include "utility/math/Vector2.h"

#include "data/graph/token_component/TokenComponentAggregation.h"

class Edge;
class QtGraphNode;

class QtGraphEdge
	: public QObject
	, public QGraphicsItemGroup
{
	Q_OBJECT
	Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
	QtGraphEdge(const std::weak_ptr<QtGraphNode>& owner, const std::weak_ptr<QtGraphNode>& target, const Edge* data, size_t weight);
	virtual ~QtGraphEdge();

	const Edge* getData() const;

	std::weak_ptr<QtGraphNode> getOwner();
	std::weak_ptr<QtGraphNode> getTarget();

	void updateLine();

	bool getIsActive() const;
	void setIsActive(bool isActive);

	void onClick();

	void focusIn();
	void focusOut();

	void setDirection(TokenComponentAggregation::Direction direction);

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
	size_t m_weight;

	TokenComponentAggregation::Direction m_direction;

	Vec2i m_mousePos;
	bool m_mouseMoved;
};

#endif // QT_GRAPH_EDGE_H
