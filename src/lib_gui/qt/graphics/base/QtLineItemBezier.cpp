#include "QtLineItemBezier.h"

#include <QPainter>

QtLineItemBezier::QtLineItemBezier(QGraphicsItem* parent)
	: QtLineItemBase(parent)
{
}

QtLineItemBezier::~QtLineItemBezier()
{
}

QPainterPath QtLineItemBezier::shape() const
{
	QPainterPathStroker stroker;
	stroker.setWidth(10);
	stroker.setJoinStyle(Qt::MiterJoin);

	QPainterPath path = stroker.createStroke(getCurve());
	if (m_showArrow)
	{
		path.addRect(getArrowBoundingRect(QtLineItemBase::getPath()).adjusted(-3, -3, 3, 3));
	}
	return path;
}

void QtLineItemBezier::paint(QPainter* painter, const QStyleOptionGraphicsItem* options, QWidget* widget)
{
	QPainterPath path = getCurve();

	if (m_showArrow)
	{
		drawArrow(QtLineItemBase::getPath(), &path);
	}

	painter->setPen(pen());
	painter->drawPath(path);
}

QPolygon QtLineItemBezier::getPath() const
{
	QPolygon poly = QtLineItemBase::getPath();
	QPolygon newPoly;

	QPoint tip = poly.at(0);

	if (m_showArrow && m_style.arrowClosed)
	{
		int dir = getDirection(poly.at(1), poly.at(0));

		switch (dir)
		{
		case 0:
			tip.ry() += m_style.arrowLength;
			break;
		case 1:
			tip.rx() -= m_style.arrowLength;
			break;
		case 2:
			tip.ry() -= m_style.arrowLength;
			break;
		case 3:
			tip.rx() += m_style.arrowLength;
			break;
		}
	}

	newPoly << tip;

	QRect rect(poly.at(0), poly.at(3));

	if (!rect.contains(poly.at(1)))
	{
		if (m_route == ROUTE_HORIZONTAL)
		{
			int diff;
			if (std::abs(poly.at(1).x() - poly.at(0).x()) < std::abs(poly.at(2).x() - poly.at(3).x()))
			{
				diff = poly.at(1).x() - poly.at(0).x();
			}
			else
			{
				diff = poly.at(2).x() - poly.at(3).x();
			}

			newPoly << QPoint(poly.at(1).x() + diff, poly.at(1).y());
			newPoly << QPoint(poly.at(2).x() + diff, poly.at(2).y());
		}
		else
		{
			int diff;
			if (std::abs(poly.at(1).y() - poly.at(0).y()) < std::abs(poly.at(2).y() - poly.at(3).y()))
			{
				diff = poly.at(1).y() - poly.at(0).y();
			}
			else
			{
				diff = poly.at(2).y() - poly.at(3).y();
			}

			newPoly << QPoint(poly.at(1).x(), poly.at(1).y() + diff);
			newPoly << QPoint(poly.at(2).x(), poly.at(2).y() + diff);
		}
	}
	else
	{
		if (m_route == ROUTE_HORIZONTAL)
		{
			newPoly << QPoint((poly.at(3).x() + poly.at(0).x()) / 2, poly.at(0).y());
			newPoly << QPoint((poly.at(3).x() + poly.at(0).x()) / 2, poly.at(3).y());
		}
		else
		{
			newPoly << QPoint(poly.at(0).x(), (poly.at(3).y() + poly.at(0).y()) / 2);
			newPoly << QPoint(poly.at(3).x(), (poly.at(3).y() + poly.at(0).y()) / 2);
		}
	}

	newPoly << poly.at(3);
	return newPoly;
}

QPainterPath QtLineItemBezier::getCurve() const
{
	QPolygon poly = getPath();

	QPainterPath path;
	path.moveTo(poly.at(3));
	path.cubicTo(
		poly.at(2),
		poly.at(1),
		poly.at(0)
	);

	// QPainterPath path;
	// path.moveTo(poly.at(3));
	// path.lineTo(poly.at(2));
	// path.lineTo(poly.at(1));
	// path.lineTo(poly.at(0));

	return path;
}
