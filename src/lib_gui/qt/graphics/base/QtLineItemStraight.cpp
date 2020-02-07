#include "QtLineItemStraight.h"

#include <QBrush>
#include <QPen>

QtLineItemStraight::QtLineItemStraight(QGraphicsItem* parent): QGraphicsLineItem(parent)
{
	this->setAcceptHoverEvents(true);
}

QtLineItemStraight::~QtLineItemStraight() {}

void QtLineItemStraight::updateLine(
	const Vec2i& origin, const Vec2i& target, const GraphViewStyle::EdgeStyle& style)
{
	prepareGeometryChange();

	setLine(origin.x(), origin.y(), target.x(), target.y());

	setPen(QPen(QBrush(style.color.c_str()), style.width, Qt::SolidLine));
}

QPainterPath QtLineItemStraight::shape() const
{
	QPainterPath path;
	QLineF l = line();

	path.moveTo(l.p1());
	path.lineTo(l.p2());

	QPainterPathStroker stroker;
	stroker.setWidth(10);
	stroker.setJoinStyle(Qt::MiterJoin);

	return stroker.createStroke(path);
}
