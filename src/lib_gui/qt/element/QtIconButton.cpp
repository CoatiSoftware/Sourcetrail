#include "qt/element/QtIconButton.h"

#include <QEvent>

#include "qt/utility/utilityQt.h"


QtHoverButton::QtHoverButton(QWidget* parent)
	: QPushButton("", parent)
{
	setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	setMouseTracking(true);
}

void QtHoverButton::enterEvent(QEvent *event)
{
	if (isEnabled())
	{
		emit hoveredIn(this);
	}
}

void QtHoverButton::leaveEvent(QEvent *event)
{
	if (isEnabled())
	{
		emit hoveredOut(this);
	}
}


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
	if (m_hoveredIconPath.size() && isEnabled())
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


QtIconStateButton::QtIconStateButton(QWidget* parent)
	: QPushButton("", parent)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	setMouseTracking(true);

	setObjectName("iconStateButton");

	leaveEvent(nullptr);
}

void QtIconStateButton::addState(ButtonState buttonState, QString iconPath, QColor color)
{
	State state;
	state.iconPath = iconPath;
	state.color = color;
	m_states.emplace(buttonState, state);

	if (buttonState == STATE_DEFAULT)
	{
		leaveEvent(nullptr);
	}
}

void QtIconStateButton::hoverIn()
{
	enterEvent(nullptr);
}

void QtIconStateButton::hoverOut()
{
	leaveEvent(nullptr);
}

void QtIconStateButton::changeEvent(QEvent *event)
{
	if (event->type() != QEvent::EnabledChange)
	{
		return;
	}

	auto it = m_states.find(isEnabled() ? STATE_DEFAULT : STATE_DISABLED);
	if (it != m_states.end())
	{
		setState(it->second);
	}
}

void QtIconStateButton::enterEvent(QEvent *event)
{
	if (!isEnabled())
	{
		return;
	}

	auto it = m_states.find(STATE_HOVERED);
	if (it != m_states.end())
	{
		setState(it->second);

		if (event)
		{
			emit hoveredIn(this);
		}
	}
}

void QtIconStateButton::leaveEvent(QEvent *event)
{
	if (!isEnabled())
	{
		return;
	}

	auto it = m_states.find(STATE_DEFAULT);
	if (it != m_states.end())
	{
		setState(it->second);

		if (event)
		{
			emit hoveredOut(this);
		}
	}
}

void QtIconStateButton::setState(State state)
{
	QPixmap pixmap = QPixmap(state.iconPath);

	if (state.color != Qt::transparent)
	{
		pixmap = utility::colorizePixmap(pixmap, state.color);
	}

	QIcon icon(pixmap);
	icon.addPixmap(pixmap, QIcon::Disabled);

	setIcon(icon);
}
