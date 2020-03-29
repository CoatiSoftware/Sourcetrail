#ifndef QT_GRAPH_EDGE_H
#define QT_GRAPH_EDGE_H

#include <memory>

#include <QGraphicsItem>

#include "Vector2.h"
#include "Vector4.h"

#include "TokenComponentAggregation.h"

class Edge;
class GraphFocusHandler;
class QtGraphNode;

class QtGraphEdge
	: public QObject
	, public QGraphicsItemGroup
{
	Q_OBJECT
	Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
	static void unfocusBezierEdge();
	static void clearFocusedEdges();

	QtGraphEdge(
		GraphFocusHandler* focusHandler,
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
	void setIsCoFocused(bool isCoFocused);

	bool isFocusable() const;

	void onClick();
	void onHide();

	void coFocusIn();
	void coFocusOut();

	void setDirection(TokenComponentAggregation::Direction direction);

	bool isHorizontal() const;
	bool isExpandable() const;

	bool isTrailEdge() const;
	void setIsTrailEdge(const std::vector<Vec4i>& path, bool horizontal);

	bool isBezierEdge() const;
	void setUseBezier(bool useBezier);
	void clearPath();

	bool isAmbiguous() const;

	QRectF getBoundingRect() const;

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

	GraphFocusHandler* m_focusHandler = nullptr;

	const Edge* m_data;

	QtGraphNode* m_owner = nullptr;
	QtGraphNode* m_target = nullptr;

	QGraphicsItem* m_child = nullptr;

	bool m_isActive = false;
	bool m_isFocused = false;
	bool m_isCoFocused = false;

	bool m_isHorizontal = false;
	size_t m_weight = 0;

	TokenComponentAggregation::Direction m_direction;

	bool m_isTrailEdge = false;
	std::vector<Vec4i> m_path;

	bool m_useBezier = false;
	bool m_isInteractive = false;

	Vec2i m_mousePos;
	bool m_mouseMoved = false;
};

#endif	  // QT_GRAPH_EDGE_H
