#ifndef QT_LINE_ITEM_BEZIER_H
#define QT_LINE_ITEM_BEZIER_H

#include "QtLineItemBase.h"

class QtLineItemBezier: public QtLineItemBase
{
public:
	QtLineItemBezier(QGraphicsItem* parent);
	virtual ~QtLineItemBezier();

	virtual QPainterPath shape() const;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* options, QWidget* widget);

protected:
	QPolygon getPath() const;

private:
	QPainterPath getCurve() const;
};

#endif	  // QT_LINE_ITEM_BEZIER_H
