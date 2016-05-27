#include "qt/graphics/QtAngledLineItem.h"

#include <cmath>

#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include "utility/math/Vector2.h"

QtAngledLineItem::QtAngledLineItem(QGraphicsItem* parent)
	: QGraphicsLineItem(parent)
	, m_route(ROUTE_ANY)
	, m_pivot(PIVOT_THIRD)
	, m_onFront(false)
	, m_onBack(false)
	, m_horizontalIn(false)
	, m_showArrow(true)
{
	this->setAcceptHoverEvents(true);
	this->setFlag(QGraphicsItem::ItemUsesExtendedStyleOption, true);
}

QtAngledLineItem::~QtAngledLineItem()
{
}

void QtAngledLineItem::updateLine(
	Vec4i ownerRect, Vec4i targetRect,
	Vec4i ownerParentRect, Vec4i targetParentRect,
	GraphViewStyle::EdgeStyle style,
	size_t weight, bool showArrow
){
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

	this->setPen(QPen(QBrush(style.color.c_str()), style.width + int(log10(weight)), Qt::SolidLine, Qt::RoundCap));
}

void QtAngledLineItem::setRoute(Route route)
{
	m_route = route;
}

void QtAngledLineItem::setPivot(Pivot pivot)
{
	m_pivot = pivot;
}

void QtAngledLineItem::setOnFront(bool front)
{
	m_onFront = front;
}

void QtAngledLineItem::setOnBack(bool back)
{
	m_onBack = back;
}

void QtAngledLineItem::setHorizontalIn(bool horizontal)
{
	m_horizontalIn = horizontal;
}

QPainterPath QtAngledLineItem::shape() const
{
	QPainterPath path;
	QPolygon poly = getPath();

	for (int i = 0; i < poly.size() - 1; i++)
	{
		path.addRect(QRectF(poly.at(i), poly.at(i + 1)).normalized().adjusted(-3, -3, 3, 3));
	}

	path.addRect(getArrowBoundingRect(poly).adjusted(-3, -3, 3, 3));

	return path;
}

void QtAngledLineItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* options, QWidget* widget)
{
	painter->setPen(pen());

	QPainterPath path;

	QPolygon poly = getPath();
	int i = poly.length() - 1;

	path.moveTo(poly.at(i));


	// while (i > 0)
	// {
	// 	i--;

	// 	path.lineTo(poly.at(i));
	// }
	// painter->drawPath(path);
	// return;


	QRectF drawRect = options->exposedRect;
	QRectF partRect;

	int radius = m_style.cornerRadius;
	int dir = getDirection(poly.at(i), poly.at(i - 1));
	while (i > 1)
	{
		i--;

		QPointF a = poly.at(i);
		QPointF b = poly.at(i - 1);

		int newDir = getDirection(a, b);
		int ar = radius;
		int br = m_style.cornerRadius;

		if (i != 1)
		{
			if (dir % 2 == 1 && std::abs(a.y() - b.y()) < 2 * br)
			{
				br = std::abs(a.y() - b.y()) / 2;
			}
			else if (dir % 2 == 0 && std::abs(a.x() - b.x()) < 2 * br)
			{
				br = std::abs(a.x() - b.x()) / 2;
			}
		}

		switch (dir)
		{
		case 0: a.setY(a.y() + ar); break;
		case 1: a.setX(a.x() - ar); break;
		case 2: a.setY(a.y() - ar); break;
		case 3: a.setX(a.x() + ar); break;
		}

		b = poly.at(i);
		switch (newDir)
		{
		case 0: b.setY(b.y() - br); break;
		case 1: b.setX(b.x() + br); break;
		case 2: b.setY(b.y() + br); break;
		case 3: b.setX(b.x() - br); break;
		}

		partRect = QRectF(poly.at(i + 1), poly.at(i)).adjusted(-1, -1, 1, 1);
		if (drawRect.intersects(partRect))
		{
			path.lineTo(a);
		}
		else
		{
			path.moveTo(a);
		}

		partRect = QRectF(a, b).normalized().adjusted(-1, -1, 1, 1);
		if (drawRect.intersects(partRect))
		{
			switch (dir)
			{
			case 0:
				if (newDir == 1)
				{
					path.arcTo(a.x(), b.y(), 2 * br, 2 * ar, 180, -90);
				}
				else if (newDir == 3)
				{
					path.arcTo(b.x() - br, a.y() - ar, 2 * br, 2 * ar, 0, 90);
				}
				break;
			case 1:
				if (newDir == 0)
				{
					path.arcTo(a.x() - ar, b.y() - br, 2 * ar, 2 * br, -90, 90);
				}
				else if (newDir == 2)
				{
					path.arcTo(a.x() - ar, a.y(), 2 * ar, 2 * br, 90, -90);
				}
				break;
			case 2:
				if (newDir == 1)
				{
					path.arcTo(a.x(), a.y() - ar, 2 * br, 2 * ar, 180, 90);
				}
				else if (newDir == 3)
				{
					path.arcTo(b.x() - br, a.y() - ar, 2 * br, 2 * ar, 0, -90);
				}
				break;
			case 3:
				if (newDir == 0)
				{
					path.arcTo(b.x(), b.y() - br, 2 * ar, 2 * br, -90, -90);
				}
				else if (newDir == 2)
				{
					path.arcTo(b.x(), a.y(), 2 * ar, 2 * br, 90, 90);
				}
				break;
			}
		}
		else
		{
			path.moveTo(b);
		}

		dir = newDir;
		radius = br;
	}

	partRect = QRectF(poly.at(0), poly.at(1)).adjusted(-1, -1, 1, 1);
	if (drawRect.intersects(partRect))
	{
		partRect = getArrowBoundingRect(poly);
		if (drawRect.intersects(partRect) && m_showArrow)
		{
			drawArrow(poly, &path);
		}
		else
		{
			path.lineTo(poly.at(0));
		}
	}

	painter->drawPath(path);
}

QPolygon QtAngledLineItem::getPath() const
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

	Vec2f o[4];
	getPivotPoints(o, oR, oR, oOff.y, false);

	Vec2f t[4];
	getPivotPoints(t, tR, tR, tOff.y, true);

	QPoint a(t[it].x, t[it].y);
	QPoint d(o[io].x, o[io].y);

	QPoint b(tP[it].x, tP[it].y);
	QPoint c(oP[io].x, oP[io].y);

	switch (it)
	{
		case 0: b.setY(b.y() - tOff.x); break;
		case 1: b.setX(b.x() + tOff.x); break;
		case 2: b.setY(b.y() + tOff.x); break;
		case 3: b.setX(b.x() - tOff.x); break;
	}

	switch (io)
	{
		case 0: c.setY(c.y() - oOff.x); break;
		case 1: c.setX(c.x() + oOff.x); break;
		case 2: c.setY(c.y() + oOff.x); break;
		case 3: c.setX(c.x() - oOff.x); break;
	}

	if (it != io)
	{
		if ((it == 1 && b.x() < c.x()) || (io == 1 && b.x() > c.x()))
		{
			if (m_horizontalIn)
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
			c.setY(b.y());
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

				a = QPoint(t[it].x, t[it].y);
				b = QPoint(tP[it].x, tP[it].y);

				switch (it)
				{
					case 0: b.setY(b.y() - tOff.x); break;
					case 1: b.setX(b.x() + tOff.x); break;
					case 2: b.setY(b.y() + tOff.x); break;
					case 3: b.setX(b.x() - tOff.x); break;
				}
			}
			else
			{
				io = (io + 2) % 4;

				d = QPoint(o[io].x, o[io].y);
				c = QPoint(oP[io].x, oP[io].y);

				switch (io)
				{
					case 0: c.setY(c.y() - oOff.x); break;
					case 1: c.setX(c.x() + oOff.x); break;
					case 2: c.setY(c.y() + oOff.x); break;
					case 3: c.setX(c.x() - oOff.x); break;
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

int QtAngledLineItem::getDirection(const QPointF& a, const QPointF& b) const
{
	if (a.x() != b.x())
	{
		if (a.x() < b.x())
		{
			return 1; // right
		}
		else
		{
			return 3; // left
		}
	}
	else
	{
		if (a.y() < b.y())
		{
			return 2; // down
		}
		else
		{
			return 0; // up
		}
	}
}

QRectF QtAngledLineItem::getArrowBoundingRect(const QPolygon& poly) const
{
	int dir = getDirection(poly.at(1), poly.at(0));

	QRectF rect(
		poly.at(0).x(),
		poly.at(0).y(),
		(dir % 2 == 1 ? m_style.arrowLength : m_style.arrowWidth),
		(dir % 2 == 0 ? m_style.arrowLength : m_style.arrowWidth)
	);

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

void QtAngledLineItem::drawArrow(const QPolygon& poly, QPainterPath* path) const
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

void QtAngledLineItem::getPivotPoints(Vec2f* p, const Vec4i& in, const Vec4i& out, int offset, bool target) const
{
	float f = m_pivot == PIVOT_THIRD ? (target ? 2 / 3.f : 1 / 3.f) : 1 / 2.f;

	p[0] = Vec2f(in.x + (in.z - in.x) * f + offset, out.y);
	p[2] = Vec2f(in.x + (in.z - in.x) * f + offset, out.w);

	p[1] = Vec2f(out.z, in.y + (in.w - in.y) * f + offset);
	p[3] = Vec2f(out.x, in.y + (in.w - in.y) * f + offset);
}
