#include "QtCountCircleItem.h"

#include <QFont>
#include <QFontMetrics>
#include <QPen>

#include "GraphViewStyle.h"

QtCountCircleItem::QtCountCircleItem(QGraphicsItem* parent): QtRoundedRectItem(parent)
{
	this->setRadius(10);
	this->setAcceptHoverEvents(true);

	QFont font;
	font.setFamily(GraphViewStyle::getFontNameOfExpandToggleNode().c_str());
	font.setPixelSize(static_cast<int>(GraphViewStyle::getFontSizeOfCountCircle()));
	font.setWeight(QFont::Normal);

	m_number = new QGraphicsSimpleTextItem(this);
	m_number->setFont(font);
}

QtCountCircleItem::~QtCountCircleItem() {}

void QtCountCircleItem::setPosition(const Vec2f& pos)
{
	qreal width = QFontMetrics(m_number->font()).width(m_number->text());
	qreal height = QFontMetrics(m_number->font()).height();

	this->setRadius(height / 2 + 1);
	this->setRect(
		pos.x - std::max(width / 2 + 4, height / 2 + 1),
		pos.y - height / 2 - 1,
		std::max(width + 8, height + 2),
		height + 2);
	m_number->setPos(pos.x - width / 2, pos.y - height / 2);
}

void QtCountCircleItem::setNumber(size_t number)
{
	const QString numberStr = QString::number(number);
	m_number->setText(numberStr);

	const QPointF center = this->rect().center();
	this->setPosition(Vec2f(static_cast<float>(center.x()), static_cast<float>(center.y())));
}

void QtCountCircleItem::setStyle(QColor color, QColor fontColor, QColor borderColor, size_t borderWidth)
{
	this->setBrush(color);
	this->setPen(QPen(borderColor, static_cast<qreal>(borderWidth)));

	m_number->setBrush(fontColor);
}
