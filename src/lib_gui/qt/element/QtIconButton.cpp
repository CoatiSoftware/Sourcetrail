#include "qt/element/QtIconButton.h"

QtIconButton::QtIconButton(QString iconPath, QString hoveredIconPath, QWidget* parent)
	: QPushButton("", parent)
	, m_iconPath(iconPath)
	, m_hoveredIconPath(hoveredIconPath)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	setMouseTracking(true);

	setObjectName("iconButton");

	leaveEvent(nullptr);
}

void QtIconButton::enterEvent(QEvent *event)
{
	if (m_hoveredIconPath.size())
	{
		setIcon(QIcon(QPixmap(m_hoveredIconPath)));
	}
}

void QtIconButton::leaveEvent(QEvent *event)
{
	if (m_iconPath.size())
	{
		setIcon(QIcon(QPixmap(m_iconPath)));
	}
}
