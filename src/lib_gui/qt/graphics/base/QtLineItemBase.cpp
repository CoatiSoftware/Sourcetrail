#include "QtLineItemBase.h"

#include <QBrush>
#include <QCursor>
#include <QPen>

QtLineItemBase::QtLineItemBase(QGraphicsItem* parent)
	: QGraphicsLineItem(parent)
	, m_showArrow(true)
	, m_onFront(false)
	, m_onBack(false)
	, m_earlyBend(false)
	, m_route(ROUTE_ANY)
{
	this->setAcceptHoverEvents(true);
	this->setCursor(Qt::PointingHandCursor);
}

QtLineItemBase::~QtLineItemBase() {}

void QtLineItemBase::updateLine(
	const Vec4i& ownerRect,
	const Vec4i& targetRect,
	const Vec4i& ownerParentRect,
	const Vec4i& targetParentRect,
	const GraphViewStyle::EdgeStyle& style,
	size_t weight,
	bool showArrow)
{
	prepareGeometryChange();
	m_polygon.clear();

	m_ownerRect = ownerRect;
	m_targetRect = targetRect;
	m_ownerParentRect = ownerParentRect;
	m_targetParentRect = targetParentRect;

	m_ownerRect.x = m_ownerRect.x - 1;
	m_ownerRect.z = m_ownerRect.z + 1;
	m_targetRect.x = m_targetRect.x - 1;
	m_targetRect.z = m_targetRect.z + 1;

	m_style = style;
	m_showArrow = showArrow;

	this->setPen(QPen(
		QBrush(style.color.c_str()), style.width + int(log10(weight)), Qt::SolidLine, Qt::RoundCap));
}

void QtLineItemBase::setRoute(Route route)
{
	m_route = route;
}

void QtLineItemBase::setOnFront(bool front)
{
	m_onFront = front;
}

void QtLineItemBase::setOnBack(bool back)
{
	m_onBack = back;
}

void QtLineItemBase::setEarlyBend(bool earlyBend)
{
	m_earlyBend = earlyBend;
}

QPolygon QtLineItemBase::getPath() const
{
	if (m_polygon.size() > 0)
	{
		return m_polygon;
	}

	const Vec4i& oR = m_ownerRect;
	const Vec4i& oPR = m_ownerParentRect;
	const Vec4i& tR = m_targetRect;
	const Vec4i& tPR = m_targetParentRect;

	const Vec2i& oOff = m_style.originOffset;
	const Vec2i& tOff = m_style.targetOffset;

	Vec2f oP[4];
	getPivotPoints(oP, oR, oPR, oOff.y, false);

	Vec2f tP[4];
	getPivotPoints(tP, tR, tPR, tOff.y, true);

	int io = -1;
	int it = -1;

	float dist = -1;
	std::map<int, float> dists;

	// find start and end points
	if (m_onFront)
	{
		io = 3;
		it = 3;
	}
	else if (m_onBack)
	{
		io = 1;
		it = 1;
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (m_route == ROUTE_HORIZONTAL && (i % 2 == 0 || j % 2 == 0))
				{
					continue;
				}
				else if (m_route == ROUTE_VERTICAL && (i % 2 == 1 || j % 2 == 1))
				{
					continue;
				}
				else if (i % 2 != j % 2)
				{
					continue;
				}

				Vec2f diff = oP[i] - tP[j];
				float d = diff.getLength();
				dists.emplace((i << 2) + j, d);

				if (dist < 0 || d < dist)
				{
					dist = d;
					io = i;
					it = j;
				}
			}
		}
	}

	// start/end and offsetted start/end points
	Vec2f o[4];
	getPivotPoints(o, oR, oR, oOff.y, false);

	Vec2f t[4];
	getPivotPoints(t, tR, tR, tOff.y, true);

	QPoint a(static_cast<int>(t[it].x), static_cast<int>(t[it].y));
	QPoint d(static_cast<int>(o[io].x), static_cast<int>(o[io].y));

	QPoint b(static_cast<int>(tP[it].x), static_cast<int>(tP[it].y));
	QPoint c(static_cast<int>(oP[io].x), static_cast<int>(oP[io].y));

	switch (it)
	{
	case 0:
		b.setY(b.y() - tOff.x);
		break;
	case 1:
		b.setX(b.x() + tOff.x);
		break;
	case 2:
		b.setY(b.y() + tOff.x);
		break;
	case 3:
		b.setX(b.x() - tOff.x);
		break;
	}

	switch (io)
	{
	case 0:
		c.setY(c.y() - oOff.x);
		break;
	case 1:
		c.setX(c.x() + oOff.x);
		break;
	case 2:
		c.setY(c.y() + oOff.x);
		break;
	case 3:
		c.setX(c.x() - oOff.x);
		break;
	}

	// move one offsetted point
	if (it != io)
	{
		if ((it == 1 && b.x() < c.x()) || (io == 1 && b.x() > c.x()))
		{
			if (m_earlyBend)
			{
				b.setX(c.x());
			}
			else
			{
				c.setX(b.x());
			}
		}
		else if ((it == 2 && b.y() < c.y()) || (io == 2 && b.y() > c.y()))
		{
			if (m_earlyBend)
			{
				b.setY(c.y());
			}
			else
			{
				c.setY(b.y());
			}
		}
		else if (
			(it == 3 && b.x() < c.x()) || (io == 3 && b.x() > c.x()) ||
			(it == 0 && b.y() < c.y()) || (io == 0 && b.y() > c.y()))
		{
			float dist1 = dists[(io << 2) + ((it + 2) % 4)];
			float dist2 = dists[(((io + 2) % 4) << 2) + it];

			if (dist1 < dist2)
			{
				it = (it + 2) % 4;

				a = QPoint(static_cast<int>(t[it].x), static_cast<int>(t[it].y));
				b = QPoint(static_cast<int>(tP[it].x), static_cast<int>(tP[it].y));

				switch (it)
				{
				case 0:
					b.setY(b.y() - tOff.x);
					break;
				case 1:
					b.setX(b.x() + tOff.x);
					break;
				case 2:
					b.setY(b.y() + tOff.x);
					break;
				case 3:
					b.setX(b.x() - tOff.x);
					break;
				}
			}
			else
			{
				io = (io + 2) % 4;

				d = QPoint(static_cast<int>(o[io].x), static_cast<int>(o[io].y));
				c = QPoint(static_cast<int>(oP[io].x), static_cast<int>(oP[io].y));

				switch (io)
				{
				case 0:
					c.setY(c.y() - oOff.x);
					break;
				case 1:
					c.setX(c.x() + oOff.x);
					break;
				case 2:
					c.setY(c.y() + oOff.x);
					break;
				case 3:
					c.setX(c.x() - oOff.x);
					break;
				}
			}
		}
	}

	if (it % 2 == 1)
	{
		if (it == io && ((it == 1 && b.x() < c.x()) || (it == 3 && b.x() > c.x())))
		{
			b.setX(c.x());
		}
		else
		{
			c.setX(b.x());
		}
	}
	else
	{
		if (it == io && ((it == 0 && b.y() > c.y()) || (it == 2 && b.y() < c.y())))
		{
			b.setY(c.y());
		}
		else
		{
			c.setY(b.y());
		}
	}

	if (it % 2 == 0)
	{
		int vo = m_style.verticalOffset;
		if (c.x() > b.x())
		{
			vo *= -1;
		}

		b.setY(b.y() + vo);
		c.setY(c.y() + vo);
	}
	else
	{
		int vo = m_style.verticalOffset;
		if (c.y() > b.y())
		{
			vo *= -1;
		}

		b.setX(b.x() + vo);
		c.setX(c.x() + vo);
	}

	QPolygon poly;
	poly << a << b << c << d;

	m_polygon = poly;

	return poly;
}

int QtLineItemBase::getDirection(QPointF a, QPointF b) const
{
	if (a.x() != b.x())
	{
		if (a.x() < b.x())
		{
			return 1;	 // right
		}
		else
		{
			return 3;	 // left
		}
	}
	else
	{
		if (a.y() < b.y())
		{
			return 2;	 // down
		}
		else
		{
			return 0;	 // up
		}
	}
}

QRectF QtLineItemBase::getArrowBoundingRect(const QPolygon& poly) const
{
	int dir = getDirection(poly.at(1), poly.at(0));

	QRectF rect(
		poly.at(0).x(),
		poly.at(0).y(),
		(dir % 2 == 1 ? m_style.arrowLength : m_style.arrowWidth),
		(dir % 2 == 0 ? m_style.arrowLength : m_style.arrowWidth));

	switch (dir)
	{
	case 0:
		rect.moveLeft(rect.left() - rect.width() / 2);
		break;
	case 1:
		rect.moveLeft(rect.left() - rect.width());
		rect.moveTop(rect.top() - rect.height() / 2);
		break;
	case 2:
		rect.moveLeft(rect.left() - rect.width() / 2);
		rect.moveTop(rect.top() - rect.height());
		break;
	case 3:
		rect.moveTop(rect.top() - rect.height() / 2);
		break;
	}

	return rect;
}

void QtLineItemBase::drawArrow(const QPolygon& poly, QPainterPath* path, QPainterPath* arrowPath) const
{
	int dir = getDirection(poly.at(1), poly.at(0));

	QPointF tip = poly.at(0);
	QPointF toBack;
	QPointF toLeft;

	switch (dir)
	{
	case 0:
		toBack.setY(m_style.arrowLength);
		toLeft.setX(-m_style.arrowWidth / 2);
		break;
	case 1:
		toBack.setX(-m_style.arrowLength);
		toLeft.setY(-m_style.arrowWidth / 2);
		break;
	case 2:
		toBack.setY(-m_style.arrowLength);
		toLeft.setX(m_style.arrowWidth / 2);
		break;
	case 3:
		toBack.setX(m_style.arrowLength);
		toLeft.setY(m_style.arrowWidth / 2);
		break;
	}

	if (m_style.arrowClosed)
	{
		path->lineTo(tip + toBack);
		path->moveTo(tip);
	}
	else
	{
		path->lineTo(tip);
	}

	if (arrowPath)
	{
		path = arrowPath;
		path->moveTo(tip);
	}

	path->lineTo(tip + toBack + toLeft);

	if (m_style.arrowClosed)
	{
		path->lineTo(tip + toBack - toLeft);
	}
	else
	{
		path->moveTo(tip + toBack - toLeft);
	}

	path->lineTo(tip);
}

void QtLineItemBase::getPivotPoints(
	Vec2f* p, const Vec4i& in, const Vec4i& out, int offset, bool target) const
{
	float f = 1 / 2.f;

	p[0] = Vec2f(static_cast<float>(in.x + (in.z - in.x) * f + offset), static_cast<float>(out.y));
	p[2] = Vec2f(static_cast<float>(in.x + (in.z - in.x) * f + offset), static_cast<float>(out.w));

	p[1] = Vec2f(static_cast<float>(out.z), static_cast<float>(in.y + (in.w - in.y) * f + offset));
	p[3] = Vec2f(static_cast<float>(out.x), static_cast<float>(in.y + (in.w - in.y) * f + offset));
}
