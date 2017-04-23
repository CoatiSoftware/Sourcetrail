#ifndef QT_GRAPH_EDGE_H
#define QT_GRAPH_EDGE_H

#include <memory>

#include <QGraphicsItem>

#include "utility/math/Vector2.h"
#include "utility/math/Vector4.h"

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
	QtGraphEdge(
		const std::weak_ptr<QtGraphNode>& owner,
		const std::weak_ptr<QtGraphNode>& target,
		const Edge* data,
		size_t weight,
		bool isActive,
		TokenComponentAggregation::Direction direction);
	virtual ~QtGraphEdge();

	const Edge* getData() const;

	std::weak_ptr<QtGraphNode> getOwner();
	std::weak_ptr<QtGraphNode> getTarget();

	void updateLine();

	bool getIsActive() const;
	void setIsActive(bool isActive);
	void setFromAndToActive(bool fromActive, bool toActive);

	void onClick();

	void focusIn();
	void focusOut();

	void setDirection(TokenComponentAggregation::Direction direction);

	bool isTrailEdge() const;
	void setIsTrailEdge(std::vector<Vec4i> path, bool horizontal);

protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

private slots:
	void dispatchMessageFocusIn();

private:
	const Edge* m_data;

	std::weak_ptr<QtGraphNode> m_owner;
	std::weak_ptr<QtGraphNode> m_target;

	QGraphicsLineItem* m_child;

	bool m_isActive;
	bool m_fromActive;
	bool m_toActive;

	bool m_isFocused;

	size_t m_weight;

	TokenComponentAggregation::Direction m_direction;

	bool m_isTrailEdge;
	bool m_isHorizontalTrail;
	std::vector<Vec4i> m_path;

	Vec2i m_mousePos;
	bool m_mouseMoved;

	bool m_willDispatchMessageFocusIn;
};

#endif // QT_GRAPH_EDGE_H
