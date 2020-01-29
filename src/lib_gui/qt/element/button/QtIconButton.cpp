#include "QtIconButton.h"

#include <QEvent>

#include "utilityQt.h"

QtIconButton::QtIconButton(const FilePath& iconPath, const FilePath& hoveredIconPath, QWidget* parent)
	: QPushButton(QLatin1String(""), parent)
	, m_iconPath(iconPath)
	, m_hoveredIconPath(hoveredIconPath)
	, m_color(Qt::transparent)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);	  // fixes layouting on Mac

	if (!m_hoveredIconPath.empty())
	{
		setMouseTracking(true);
	}

	leaveEvent(nullptr);
}

void QtIconButton::setColor(QColor color)
{
	m_color = color;
	leaveEvent(nullptr);
}

void QtIconButton::enterEvent(QEvent* event)
{
	if (!m_hoveredIconPath.empty() && isEnabled())
	{
		setIconFromPath(m_hoveredIconPath);
	}
}

void QtIconButton::leaveEvent(QEvent* event)
{
	if (!m_iconPath.empty())
	{
		setIconFromPath(m_iconPath);
	}
}

void QtIconButton::setIconFromPath(const FilePath& path)
{
	QPixmap pixmap = QPixmap(QString::fromStdWString(path.wstr()));

	if (m_color != Qt::transparent)
	{
		pixmap = utility::colorizePixmap(pixmap, m_color);
	}

	setIcon(QIcon(pixmap));
}
