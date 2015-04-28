#include "qt/graphics/QtStraightLineItem.h"

#include <QFont>
#include <QFontMetrics>
#include <QPen>

#include "utility/math/Vector2.h"
#include "utility/utility.h"

#include "qt/graphics/QtRoundedRectItem.h"

QtStraightLineItem::QtStraightLineItem(QGraphicsItem* parent)
	: QGraphicsLineItem(parent)
{
	this->setAcceptHoverEvents(true);

	m_circle = new QtRoundedRectItem(this);
	m_circle->setRadius(10);
	m_circle->setBrush(QBrush(QColor("#FFF")));
	m_circle->setAcceptHoverEvents(true);

	QFont font;
	font.setFamily(GraphViewStyle::getFontNameOfExpandToggleNode().c_str());
	font.setPixelSize(GraphViewStyle::getFontSizeOfExpandToggleNode());
	font.setWeight(QFont::Normal);

	m_number = new QGraphicsSimpleTextItem(this);
	m_number->setFont(font);
}

QtStraightLineItem::~QtStraightLineItem()
{
}

void QtStraightLineItem::updateLine(Vec4i ownerRect, Vec4i targetRect, int number, GraphViewStyle::EdgeStyle style)
{
	prepareGeometryChange();

	const Vec4i& o = ownerRect;
	const Vec4i& t = targetRect;

	Vec2f oc((o.x + o.z) / 2, (o.y + o.w) / 2);
	Vec2f tc((t.x + t.z) / 2, (t.y + t.w) / 2);

	Vec2f mid;
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

	mid = (op + tp) / 2;

	this->setLine(oc.x, oc.y, tc.x, tc.y);

	m_circle->setRect(mid.x - 10, mid.y - 10, 20, 20);
	m_circle->setPen(QPen(QColor(style.color.c_str()), 2));

	QString numberStr = QString::number(number);
	m_number->setText(numberStr);
	m_number->setBrush(QBrush(QColor("#666")));
	m_number->setPos(
		mid.x - QFontMetrics(m_number->font()).width(numberStr) / 2,
		mid.y - QFontMetrics(m_number->font()).height() / 2
	);

	this->setPen(QPen(QBrush(style.color.c_str()), number + style.width, Qt::SolidLine, Qt::RoundCap));
}
