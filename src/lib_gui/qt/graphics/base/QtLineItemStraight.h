#ifndef QT_LINE_ITEM_STRAIGHT_H
#define QT_LINE_ITEM_STRAIGHT_H

#include <QGraphicsItem>

#include "GraphViewStyle.h"

class QtLineItemStraight: public QGraphicsLineItem
{
public:
	QtLineItemStraight(QGraphicsItem* parent);
	virtual ~QtLineItemStraight();

	void updateLine(const Vec2i& origin, const Vec2i& target, const GraphViewStyle::EdgeStyle& style);

	virtual QPainterPath shape() const;
};

#endif	  // QT_LINE_ITEM_STRAIGHT_H
