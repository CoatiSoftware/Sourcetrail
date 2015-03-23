#ifndef QT_STRAIGHT_LINE_ITEM_H
#define QT_STRAIGHT_LINE_ITEM_H

#include <QGraphicsItem>

#include "utility/math/Vector4.h"

#include "component/view/GraphViewStyle.h"

class QtRoundedRectItem;

class QtStraightLineItem
	: public QGraphicsLineItem
{
public:
	QtStraightLineItem(QGraphicsItem* parent);
	virtual ~QtStraightLineItem();

	void updateLine(Vec4i ownerRect, Vec4i targetRect, int number, GraphViewStyle::EdgeStyle style);

private:
	QtRoundedRectItem* m_circle;
	QGraphicsSimpleTextItem* m_number;
};

#endif // QT_STRAIGHT_LINE_ITEM_H
