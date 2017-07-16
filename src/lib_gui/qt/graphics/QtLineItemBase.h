#ifndef QT_LINE_ITEM_BASE_H
#define QT_LINE_ITEM_BASE_H

#include <QGraphicsItem>

#include "utility/math/Vector4.h"

#include "component/view/GraphViewStyle.h"

class QtLineItemBase
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

	QtLineItemBase(QGraphicsItem* parent);
	virtual ~QtLineItemBase();

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

protected:
	QPolygon getPath() const;
	int getDirection(const QPointF& a, const QPointF& b) const;

	QRectF getArrowBoundingRect(const QPolygon& poly) const;
	void drawArrow(const QPolygon& poly, QPainterPath* path, QPainterPath* arrowPath = nullptr) const;

	void getPivotPoints(Vec2f* p, const Vec4i& in, const Vec4i& out, int offset, bool target) const;

	GraphViewStyle::EdgeStyle m_style;
	bool m_showArrow;

	bool m_onFront;
	bool m_onBack;
	bool m_horizontalIn;

	Route m_route;
	Pivot m_pivot;

private:
	Vec4i m_ownerRect;
	Vec4i m_targetRect;

	Vec4i m_ownerParentRect;
	Vec4i m_targetParentRect;

	mutable QPolygon m_polygon;
};

#endif // QT_LINE_ITEM_BASE_H
