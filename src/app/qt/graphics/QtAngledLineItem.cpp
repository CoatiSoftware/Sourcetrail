#include "qt/graphics/QtAngledLineItem.h"

#include <cmath>

#include <QPainter>

#include "utility/math/Vector2.h"

QtAngledLineItem::QtAngledLineItem(QGraphicsItem* parent)
	: QGraphicsLineItem(parent)
{
	this->setAcceptHoverEvents(true);
}

QtAngledLineItem::~QtAngledLineItem()
{
}

void QtAngledLineItem::updateLine(
	Vec4i ownerRect, Vec4i targetRect,
	Vec4i ownerParentRect, Vec4i targetParentRect,
	GraphViewStyle::EdgeStyle style
){
	prepareGeometryChange();

	m_ownerRect = ownerRect;
	m_targetRect = targetRect;
	m_ownerParentRect = ownerParentRect;
	m_targetParentRect = targetParentRect;

	m_ownerRect.x = m_ownerRect.x - 1;
	m_ownerRect.z = m_ownerRect.z + 1;
	m_targetRect.x = m_targetRect.x - 1;
	m_targetRect.z = m_targetRect.z + 1;

	m_style = style;

	this->setPen(QPen(QBrush(style.color.c_str()), style.width, Qt::SolidLine, Qt::RoundCap));
}

QPainterPath QtAngledLineItem::shape() const
{
	int w = m_style.arrowWidth / 2 + 1;
	QPainterPath path;
	QPolygon poly = getPath();
	for (int i = 0; i < poly.size() - 1; i++)
	{
		path.addRect(QRectF(poly.at(i), poly.at(i + 1)).normalized().adjusted(-w, -w, w, w));
	}
	return path;
}

void QtAngledLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem* options, QWidget* widget)
{
	painter->setPen(pen());

	QPainterPath path;

	QPolygon poly = getPath();
	int i = poly.length() - 1;

	path.moveTo(poly.at(i));

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

		dir = newDir;
		radius = br;
	}

	QPointF arrow = poly.at(0) + QPointF((poly.at(0).x() - poly.at(1).x() > 0 ? -1 : 1) * m_style.arrowLength, 0);

	if (m_style.arrowClosed)
	{
		path.lineTo(arrow);
		path.moveTo(poly.at(0));
	}
	else
	{
		path.lineTo(poly.at(0));
	}

	arrow.setY(arrow.y() - m_style.arrowWidth / 2);
	path.lineTo(arrow);
	arrow.setY(arrow.y() + m_style.arrowWidth);

	if (m_style.arrowClosed)
	{
		path.lineTo(arrow);
	}
	else
	{
		path.moveTo(arrow);
	}
	path.lineTo(poly.at(0));

	painter->drawPath(path);
}

QPolygon QtAngledLineItem::getPath() const
{
	const Vec4i& oR = m_ownerRect;
	const Vec4i& tR = m_targetRect;

	Vec2f o[2] = { Vec2f(m_ownerParentRect.x, (oR.y + 2 * oR.w) / 3), Vec2f(m_ownerParentRect.z, (oR.y + 2 * oR.w) / 3) };
	Vec2f t[2] = { Vec2f(m_targetParentRect.x, (2 * tR.y + tR.w) / 3), Vec2f(m_targetParentRect.z, (2 * tR.y + tR.w) / 3) };

	int io = -1;
	int it = -1;
	float dist = -1;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			Vec2f diff = o[i] - t[j];
			if (dist < 0 || diff.getLength() < dist)
			{
				dist = diff.getLength();
				io = i;
				it = j;
			}
		}
	}

	QPoint tp((it ? 1 : -1) * m_style.targetOffset.x + t[it].x, t[it].y + m_style.targetOffset.y);
	QPoint op((io ? 1 : -1) * m_style.originOffset.x + o[io].x, o[io].y + m_style.originOffset.y);

	if (it != io)
	{
		if (it && tp.x() < op.x())
		{
			io = 0;
			op = QPoint(o[io].x - m_style.originOffset.x, o[io].y + m_style.originOffset.y);
		}
		else if (!it && tp.x() < op.x())
		{
			it = 1;
			tp = QPoint(t[it].x + m_style.targetOffset.x, t[it].y + m_style.targetOffset.y);
		}
		else if (io && tp.x() > op.x())
		{
			io = 1;
			op = QPoint(o[io].x + m_style.originOffset.x, o[io].y + m_style.originOffset.y);
		}
		else if (!io && tp.x() > op.x())
		{
			it = 0;
			tp = QPoint(t[it].x - m_style.targetOffset.x, t[it].y + m_style.targetOffset.y);
		}
	}

	if (it == io && ((it && tp.x() < op.x()) || (!it && tp.x() > op.x())) )
	{
		tp.setX(op.x());
	}
	else
	{
		op.setX(tp.x());
	}

	o[0] = Vec2f(oR.x, (oR.y + 2 * oR.w) / 3);
	o[1] = Vec2f(oR.z, (oR.y + 2 * oR.w) / 3);
	t[0] = Vec2f(tR.x, (2 * tR.y + tR.w) / 3);
	t[1] = Vec2f(tR.z, (2 * tR.y + tR.w) / 3);

	if (o[io].y < t[it].y)
	{
		op.setX(op.x() + m_style.verticalOffset);
		tp.setX(tp.x() + m_style.verticalOffset);
	}
	else
	{
		op.setX(op.x() - m_style.verticalOffset);
		tp.setX(tp.x() - m_style.verticalOffset);
	}

	QPolygon poly;
	poly << QPoint(t[it].x, t[it].y + m_style.targetOffset.y);
	poly << tp;
	poly << op;
	poly << QPoint(o[io].x, o[io].y + m_style.originOffset.y);
	return poly;
}

int QtAngledLineItem::getDirection(const QPointF& a, const QPointF& b) const
{
	if (a.x() != b.x())
	{
		if (a.x() < b.x())
		{
			return 1;
		}
		else
		{
			return 3;
		}
	}
	else
	{
		if (a.y() < b.y())
		{
			return 2;
		}
		else
		{
			return 0;
		}
	}
}
