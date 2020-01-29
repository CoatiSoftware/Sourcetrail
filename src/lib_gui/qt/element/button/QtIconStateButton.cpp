#include "QtIconStateButton.h"

#include <QEvent>

#include "utilityQt.h"

QtIconStateButton::QtIconStateButton(QWidget* parent): QPushButton(QLatin1String(""), parent)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);	  // fixes layouting on Mac
	setMouseTracking(true);

	setObjectName(QStringLiteral("iconStateButton"));

	leaveEvent(nullptr);
}

void QtIconStateButton::addState(ButtonState buttonState, const FilePath& iconPath, QColor color)
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

void QtIconStateButton::changeEvent(QEvent* event)
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

void QtIconStateButton::enterEvent(QEvent* event)
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

void QtIconStateButton::leaveEvent(QEvent* event)
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
	QPixmap pixmap = QPixmap(QString::fromStdWString(state.iconPath.wstr()));

	if (state.color != Qt::transparent)
	{
		pixmap = utility::colorizePixmap(pixmap, state.color);
	}

	QIcon icon(pixmap);
	icon.addPixmap(pixmap, QIcon::Disabled);

	setIcon(icon);
}
