#include "qt/graphics/QtStraightLineItem.h"

#include <QBrush>
#include <QPen>

#include "utility/math/Vector2.h"
#include "utility/utility.h"

#include "qt/graphics/QtCountCircleItem.h"

QtStraightLineItem::QtStraightLineItem(QGraphicsItem* parent)
	: QGraphicsLineItem(parent)
{
	this->setAcceptHoverEvents(true);

	m_circle = new QtCountCircleItem(this);

	m_arrowLeft = new QGraphicsLineItem(this);
	m_arrowRight = new QGraphicsLineItem(this);
}

QtStraightLineItem::~QtStraightLineItem()
{
}

void QtStraightLineItem::updateLine(
	Vec4i ownerRect, Vec4i targetRect, int number,
	GraphViewStyle::EdgeStyle style, GraphViewStyle::NodeStyle countStyle, bool showArrow
){
	prepareGeometryChange();

	const Vec4i& o = ownerRect;
	const Vec4i& t = targetRect;

	Vec2f oc((o.x + o.z) / 2, (o.y + o.w) / 2);
	Vec2f tc((t.x + t.z) / 2, (t.y + t.w) / 2);

	Vec2f op;
	Vec2f tp;

	bool intersects = false;
	for (int i = 0; !intersects && i < 4; i++)
	{
		switch (i)
		{
			case 0: intersects = utility::intersectionPoint(oc, tc, Vec2f(o.x, o.y), Vec2f(o.x, o.w), &op); break;
			case 1: intersects = utility::intersectionPoint(oc, tc, Vec2f(o.x, o.y), Vec2f(o.z, o.y), &op); break;
			case 2: intersects = utility::intersectionPoint(oc, tc, Vec2f(o.z, o.y), Vec2f(o.z, o.w), &op); break;
			case 3: intersects = utility::intersectionPoint(oc, tc, Vec2f(o.x, o.w), Vec2f(o.z, o.w), &op); break;
		}
	}

	if (!intersects)
	{
		op = oc;
	}

	intersects = false;
	for (int i = 0; !intersects && i < 4; i++)
	{
		switch (i)
		{
			case 0: intersects = utility::intersectionPoint(oc, tc, Vec2f(t.x, t.y), Vec2f(t.x, t.w), &tp); break;
			case 1: intersects = utility::intersectionPoint(oc, tc, Vec2f(t.x, t.y), Vec2f(t.z, t.y), &tp); break;
			case 2: intersects = utility::intersectionPoint(oc, tc, Vec2f(t.z, t.y), Vec2f(t.z, t.w), &tp); break;
			case 3: intersects = utility::intersectionPoint(oc, tc, Vec2f(t.x, t.w), Vec2f(t.z, t.w), &tp); break;
		}
	}

	if (!intersects)
	{
		tp = tc;
	}

	this->setLine(oc.x, oc.y, tc.x, tc.y);

	Vec2f mid = (op + tp) / 2;
	m_circle->setPosition(mid);
	m_circle->setNumber(number);

	size_t radius = m_circle->getRadius();

	if (showArrow)
	{
		Vec2f unit = (tp - op).normalize();
		Vec2f nUnit(-unit.y, unit.x);
		Vec2f arrow = mid + unit * (radius + 13);

		Vec2f arrowSide = mid + unit * (radius + 6) + nUnit * 7;
		m_arrowRight->setLine(arrow.x, arrow.y, arrowSide.x, arrowSide.y);

		arrowSide -= nUnit * 14;
		m_arrowLeft->setLine(arrow.x, arrow.y, arrowSide.x, arrowSide.y);

		m_arrowLeft->show();
		m_arrowRight->show();
	}
	else
	{
		m_arrowLeft->hide();
		m_arrowRight->hide();
	}

	this->setPen(QPen(QBrush(style.color.c_str()), style.width, Qt::SolidLine, Qt::RoundCap));

	m_circle->setStyle(countStyle.color.fill.c_str(), countStyle.color.text.c_str(), countStyle.color.border.c_str(), 1);

	m_arrowLeft->setPen(QPen(QBrush(countStyle.color.border.c_str()), 1, Qt::SolidLine, Qt::RoundCap));
	m_arrowRight->setPen(QPen(QBrush(countStyle.color.border.c_str()), 1, Qt::SolidLine, Qt::RoundCap));
}
