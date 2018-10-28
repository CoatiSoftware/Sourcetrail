#include "QtIconButton.h"

#include <QEvent>

#include "utilityQt.h"
#include "ColorScheme.h"


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


QtIconButton::QtIconButton(const FilePath& iconPath, const FilePath& hoveredIconPath, QWidget* parent)
	: QPushButton("", parent)
	, m_iconPath(iconPath)
	, m_hoveredIconPath(hoveredIconPath)
	, m_color(Qt::transparent)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac

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

void QtIconButton::enterEvent(QEvent *event)
{
	if (!m_hoveredIconPath.empty() && isEnabled())
	{
		setIconFromPath(m_hoveredIconPath);
	}
}

void QtIconButton::leaveEvent(QEvent *event)
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


QtSelfRefreshIconButton::QtSelfRefreshIconButton(
	const QString& text, const FilePath& iconPath, const std::string& buttonKey, QWidget* parent
)
	: QPushButton(text, parent)
	, m_iconPath(iconPath)
	, m_buttonKey(buttonKey)
{
	setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	refresh();
}

void QtSelfRefreshIconButton::setIconPath(const FilePath& iconPath)
{
	if (iconPath != m_iconPath)
	{
		m_iconPath = iconPath;
		refresh();
	}
}

void QtSelfRefreshIconButton::handleMessage(MessageRefreshUI* message)
{
	m_onQtThread([this]()
	{
		refresh();
	});
}

void QtSelfRefreshIconButton::refresh()
{
	if (!m_iconPath.empty())
	{
		setIcon(utility::createButtonIcon(m_iconPath, m_buttonKey));
	}
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
	QPixmap pixmap = QPixmap(QString::fromStdWString(state.iconPath.wstr()));

	if (state.color != Qt::transparent)
	{
		pixmap = utility::colorizePixmap(pixmap, state.color);
	}

	QIcon icon(pixmap);
	icon.addPixmap(pixmap, QIcon::Disabled);

	setIcon(icon);
}
