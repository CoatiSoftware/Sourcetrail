#include "qt/graphics/QtCountCircleItem.h"

#include <QFont>
#include <QFontMetrics>
#include <QPen>

#include "component/view/GraphViewStyle.h"

QtCountCircleItem::QtCountCircleItem(QGraphicsItem* parent)
	: QtRoundedRectItem(parent)
{
	this->setRadius(100);
	this->setAcceptHoverEvents(true);

	QFont font;
	font.setFamily(GraphViewStyle::getFontNameOfExpandToggleNode().c_str());
	font.setPixelSize(GraphViewStyle::getFontSizeOfCountCircle());
	font.setWeight(QFont::Normal);

	m_number = new QGraphicsSimpleTextItem(this);
	m_number->setFont(font);
}

QtCountCircleItem::~QtCountCircleItem()
{
}

void QtCountCircleItem::setPosition(const Vec2f& pos)
{
	qreal radius = getRadius();
	this->setRect(pos.x - radius, pos.y - radius, 2 * radius, 2 * radius);

	m_number->setPos(
		pos.x - radius + 5,
		pos.y - QFontMetrics(m_number->font()).height() / 2
	);
}

void QtCountCircleItem::setNumber(size_t number)
{
	QString numberStr = QString::number(number);
	m_number->setText(numberStr);

	qreal radius = QFontMetrics(m_number->font()).width(numberStr) / 2 + 5;
	this->setRadius(radius);

	QPointF center = this->rect().center();
	this->setPosition(Vec2f(center.x(), center.y()));
}

void QtCountCircleItem::setStyle(QColor color, QColor fontColor, QColor borderColor, size_t borderWidth)
{
	this->setBrush(color);
	this->setPen(QPen(borderColor, borderWidth));

	m_number->setBrush(fontColor);
}
