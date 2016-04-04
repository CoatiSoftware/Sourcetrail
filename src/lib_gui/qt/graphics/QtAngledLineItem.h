#ifndef QT_ANGLED_LINE_ITEM_H
#define QT_ANGLED_LINE_ITEM_H

#include <QGraphicsItem>

#include "utility/math/Vector4.h"

#include "component/view/GraphViewStyle.h"

class QtAngledLineItem
	: public QGraphicsLineItem
{
public:
	enum Route
	{
		ROUTE_ANY,
		ROUTE_HORIZONTAL,
		ROUTE_VERTICAL
	};

	enum Pivot
	{
		PIVOT_THIRD,
		PIVOT_MIDDLE
	};

	QtAngledLineItem(QGraphicsItem* parent);
	virtual ~QtAngledLineItem();

	void updateLine(
		Vec4i ownerRect, Vec4i targetRect,
		Vec4i ownerParentRect, Vec4i targetParentRect,
		GraphViewStyle::EdgeStyle style,
		size_t weight, bool showArrow);

	void setRoute(Route route);
	void setPivot(Pivot pivot);

	void setOnFront(bool front);
	void setOnBack(bool back);
	void setHorizontalIn(bool horizontal);

	virtual QPainterPath shape() const;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* options, QWidget* widget);

private:
	QPolygon getPath() const;
	int getDirection(const QPointF& a, const QPointF& b) const;

	QRectF getArrowBoundingRect(const QPolygon& poly) const;
	void drawArrow(const QPolygon& poly, QPainterPath* path) const;

	void getPivotPoints(Vec2f* p, const Vec4i& in, const Vec4i& out, int offset, bool target) const;

	Vec4i m_ownerRect;
	Vec4i m_targetRect;

	Vec4i m_ownerParentRect;
	Vec4i m_targetParentRect;

	GraphViewStyle::EdgeStyle m_style;

	Route m_route;
	Pivot m_pivot;

	bool m_onFront;
	bool m_onBack;
	bool m_horizontalIn;
	bool m_showArrow;

	mutable QPolygon m_polygon;
};

#endif // QT_ANGLED_LINE_ITEM_H
