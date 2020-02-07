#ifndef QT_GRAPH_EDGE_H
#define QT_GRAPH_EDGE_H

#include <memory>

#include <QGraphicsItem>

#include "Vector2.h"
#include "Vector4.h"

#include "TokenComponentAggregation.h"

class Edge;
class QtGraphNode;

class QtGraphEdge
	: public QObject
	, public QGraphicsItemGroup
{
	Q_OBJECT
	Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
	static void unfocusBezierEdge();

	QtGraphEdge(
		QtGraphNode* owner,
		QtGraphNode* target,
		const Edge* data,
		size_t weight,
		bool isActive,
		bool isInteractive,
		bool horizontal,
		TokenComponentAggregation::Direction direction);
	virtual ~QtGraphEdge();

	const Edge* getData() const;

	QtGraphNode* getOwner();
	QtGraphNode* getTarget();

	Id getTokenId() const;

	void updateLine();

	bool getIsActive() const;
	void setIsActive(bool isActive);

	void setIsFocused(bool isFocused);

	void onClick();
	void onHide();

	void focusIn();
	void focusOut();

	void setDirection(TokenComponentAggregation::Direction direction);

	bool isTrailEdge() const;
	void setIsTrailEdge(const std::vector<Vec4i>& path, bool horizontal);

	void setUseBezier(bool useBezier);
	void clearPath();

	bool isAmbiguous() const;

protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

private:
	// used to send tooltip message on focusIn(), because both focus messages are filtered out if
	// sent close together
	static QtGraphEdge* s_focusedEdge;

	// used to unfocus recent edge, because hover leave event is not always received for bezier edges
	static QtGraphEdge* s_focusedBezierEdge;

	const Edge* m_data;

	QtGraphNode* m_owner;
	QtGraphNode* m_target;

	QGraphicsItem* m_child;

	bool m_isActive;
	bool m_isFocused;

	bool m_isHorizontal;
	size_t m_weight;

	TokenComponentAggregation::Direction m_direction;

	bool m_isTrailEdge;
	std::vector<Vec4i> m_path;

	bool m_useBezier;
	bool m_isInteractive;

	Vec2i m_mousePos;
	bool m_mouseMoved;
};

#endif	  // QT_GRAPH_EDGE_H
