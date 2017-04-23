#ifndef QT_LINE_ITEM_STRAIGHT_H
#define QT_LINE_ITEM_STRAIGHT_H

#include <QGraphicsItem>

#include "component/view/GraphViewStyle.h"

class QtLineItemStraight
	: public QGraphicsLineItem
{
public:
	QtLineItemStraight(QGraphicsItem* parent);
	virtual ~QtLineItemStraight();

	void updateLine(Vec2i origin, Vec2i target, GraphViewStyle::EdgeStyle style);

	virtual QPainterPath shape() const;
};

#endif // QT_LINE_ITEM_STRAIGHT_H
