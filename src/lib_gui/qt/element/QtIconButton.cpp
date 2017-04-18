#include "qt/element/QtIconButton.h"

#include "qt/utility/utilityQt.h"

QtIconButton::QtIconButton(QString iconPath, QString hoveredIconPath, QWidget* parent)
	: QPushButton("", parent)
	, m_iconPath(iconPath)
	, m_hoveredIconPath(hoveredIconPath)
	, m_color(Qt::transparent)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	setMouseTracking(true);

	setObjectName("iconButton");

	leaveEvent(nullptr);
}

void QtIconButton::setColor(QColor color)
{
	m_color = color;
	leaveEvent(nullptr);
}

void QtIconButton::enterEvent(QEvent *event)
{
	if (m_hoveredIconPath.size())
	{
		setIconFromPath(m_hoveredIconPath);
	}
}

void QtIconButton::leaveEvent(QEvent *event)
{
	if (m_iconPath.size())
	{
		setIconFromPath(m_iconPath);
	}
}

void QtIconButton::setIconFromPath(QString path)
{
	QPixmap pixmap = QPixmap(path);

	if (m_color != Qt::transparent)
	{
		pixmap = utility::colorizePixmap(pixmap, m_color);
	}

	setIcon(QIcon(pixmap));
}
