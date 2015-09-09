#ifndef QT_COUNT_CIRCLE_ITEM_H
#define QT_COUNT_CIRCLE_ITEM_H

#include "utility/math/Vector2.h"

#include "qt/graphics/QtRoundedRectItem.h"

class QtCountCircleItem
	: public QtRoundedRectItem
{
public:
	QtCountCircleItem(QGraphicsItem* parent);
	virtual ~QtCountCircleItem();

	void setPosition(const Vec2f& pos);
	void setNumber(size_t number);
	void setStyle(QColor color, QColor fontColor, QColor borderColor, size_t borderWidth);

private:
	QGraphicsSimpleTextItem* m_number;
};

#endif // QT_COUNT_CIRCLE_ITEM_H
