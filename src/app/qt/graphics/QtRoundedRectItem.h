#ifndef QT_GRAPHICS_ROUNDED_RECT_ITEM_H
#define QT_GRAPHICS_ROUNDED_RECT_ITEM_H

#include <QGraphicsRectItem>

class QtRoundedRectItem
	: public QGraphicsRectItem
{
public:
	QtRoundedRectItem(QGraphicsItem* parent);
	virtual ~QtRoundedRectItem();

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem* options, QWidget* widget);

	void setShadow(QColor color, int blurRadius);
	void setShadowEnabled(bool enabled);

	qreal getRadius() const;
	void setRadius(qreal radius);

private:
	qreal m_radius;
};

#endif // QT_GRAPHICS_ROUNDED_RECT_ITEM_H
