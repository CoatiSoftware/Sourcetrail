#ifndef QT_LINE_ITEM_ANGLED_H
#define QT_LINE_ITEM_ANGLED_H

#include "QtLineItemBase.h"

class QtLineItemAngled: public QtLineItemBase
{
public:
	QtLineItemAngled(QGraphicsItem* parent);
	virtual ~QtLineItemAngled();

	virtual QPainterPath shape() const;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* options, QWidget* widget);
};

#endif	  // QT_LINE_ITEM_ANGLED_H
