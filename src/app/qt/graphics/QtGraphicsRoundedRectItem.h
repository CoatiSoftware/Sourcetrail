#ifndef QT_GRAPHICS_ROUNDED_RECT_ITEM_H
#define QT_GRAPHICS_ROUNDED_RECT_ITEM_H

#include <QGraphicsRectItem>

class QtGraphicsRoundedRectItem
	: public QGraphicsRectItem
{
public:
	QtGraphicsRoundedRectItem(QGraphicsItem* parent);
	virtual ~QtGraphicsRoundedRectItem();

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem* options, QWidget* widget);

	void setShadow(QColor color, int blurRadius);
	void setRadius(qreal radius);

private:
	qreal m_radius;
};

#endif // QT_GRAPHICS_ROUNDED_RECT_ITEM_H
