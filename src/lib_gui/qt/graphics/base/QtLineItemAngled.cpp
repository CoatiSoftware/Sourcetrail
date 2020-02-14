#include "QtLineItemAngled.h"

#include <cmath>

#include <QPainter>
#include <QStyleOptionGraphicsItem>

QtLineItemAngled::QtLineItemAngled(QGraphicsItem* parent): QtLineItemBase(parent)
{
	this->setFlag(QGraphicsItem::ItemUsesExtendedStyleOption, true);
}

QtLineItemAngled::~QtLineItemAngled() {}

QPainterPath QtLineItemAngled::shape() const
{
	QPainterPath path;
	path.setFillRule(Qt::WindingFill);

	QPolygon poly = getPath();

	for (int i = 0; i < poly.size() - 1; i++)
	{
		path.addRect(QRectF(poly.at(i), poly.at(i + 1)).normalized().adjusted(-5, -5, 5, 5));
	}

	path.addRect(getArrowBoundingRect(poly).adjusted(-3, -3, 3, 3));

	return path;
}

void QtLineItemAngled::paint(QPainter* painter, const QStyleOptionGraphicsItem* options, QWidget* widget)
{
	QPen p = pen();
	painter->setPen(p);

	QPainterPath path;

	QPolygon poly = getPath();
	int i = poly.length() - 1;

	path.moveTo(poly.at(i));


	// debug: draw line path
	// while (i > 0)
	// {
	// 	i--;

	// 	path.lineTo(poly.at(i));
	// }
	// painter->drawPath(path);
	// return;


	// debug: draw bounding rects
	// path = shape();
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
				br = static_cast<int>(std::abs(a.y() - b.y()) / 2);
			}
			else if (dir % 2 == 0 && std::abs(a.x() - b.x()) < 2 * br)
			{
				br = static_cast<int>(std::abs(a.x() - b.x()) / 2);
			}
		}

		switch (dir)
		{
		case 0:
			a.setY(a.y() + ar);
			break;
		case 1:
			a.setX(a.x() - ar);
			break;
		case 2:
			a.setY(a.y() - ar);
			break;
		case 3:
			a.setX(a.x() + ar);
			break;
		}

		b = poly.at(i);
		switch (newDir)
		{
		case 0:
			b.setY(b.y() - br);
			break;
		case 1:
			b.setX(b.x() + br);
			break;
		case 2:
			b.setY(b.y() + br);
			break;
		case 3:
			b.setX(b.x() - br);
			break;
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
			if (m_style.dashed)
			{
				QPainterPath arrowPath;
				drawArrow(poly, &path, &arrowPath);

				painter->drawPath(arrowPath);
			}
			else
			{
				drawArrow(poly, &path);
			}
		}
		else
		{
			path.lineTo(poly.at(0));
		}
	}

	if (m_style.dashed)
	{
		p.setStyle(Qt::DashLine);
		painter->setPen(p);
	}

	painter->drawPath(path);
}
