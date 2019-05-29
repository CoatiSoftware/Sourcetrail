#include "QtRoundedRectItem.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>

QtRoundedRectItem::QtRoundedRectItem(QGraphicsItem* parent)
	: QGraphicsRectItem(parent)
	, m_radius(0.0f)
{
	this->setZValue(-1.0f);
}

QtRoundedRectItem::~QtRoundedRectItem()
{
}

void QtRoundedRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem* options, QWidget* widget)
{
	painter->setPen(pen());
	painter->setBrush(brush());

	painter->setRenderHint(QPainter::Antialiasing);

	painter->drawRoundedRect(this->rect(), m_radius, m_radius);
}

void QtRoundedRectItem::setShadow(QColor color, int blurRadius)
{
	QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
	effect->setColor(color);
	effect->setBlurRadius(5);
	effect->setOffset(0, 0);
	this->setGraphicsEffect(effect);
}

void QtRoundedRectItem::setShadowEnabled(bool enabled)
{
	if (this->graphicsEffect())
	{
		this->graphicsEffect()->setEnabled(enabled);
	}
}

qreal QtRoundedRectItem::getRadius() const
{
	return m_radius;
}

void QtRoundedRectItem::setRadius(qreal radius)
{
	m_radius = radius;
}
