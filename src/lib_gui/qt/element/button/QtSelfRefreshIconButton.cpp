#include "QtSelfRefreshIconButton.h"

#include <QResizeEvent>

#include "../../utility/utilityQt.h"

QtSelfRefreshIconButton::QtSelfRefreshIconButton(
	const QString& text, const FilePath& iconPath, const std::string& buttonKey, QWidget* parent)
	: QPushButton(text, parent), m_text(text), m_iconPath(iconPath), m_buttonKey(buttonKey)
{
	setAttribute(Qt::WA_LayoutUsesWidgetRect);	  // fixes layouting on Mac
	refresh();

	m_updateTimer.setSingleShot(true);
	m_blockTimer.setSingleShot(true);
	connect(&m_updateTimer, &QTimer::timeout, [this]() { updateText(width()); });
	connect(&m_blockTimer, &QTimer::timeout, [this]() { m_blockUpdate = false; });
}

void QtSelfRefreshIconButton::setText(const QString& text)
{
	m_text = text;
	QPushButton::setText(text);

	if (m_autoElide)
	{
		m_updateTimer.start(25);
	}
}

void QtSelfRefreshIconButton::setIconPath(const FilePath& iconPath)
{
	if (iconPath != m_iconPath)
	{
		m_iconPath = iconPath;
		refresh();
	}
}

void QtSelfRefreshIconButton::setAutoElide(bool autoElide)
{
	m_autoElide = autoElide;
}

void QtSelfRefreshIconButton::handleMessage(MessageRefreshUI* message)
{
	m_onQtThread([this]() { refresh(); });
}

void QtSelfRefreshIconButton::refresh()
{
	if (!m_iconPath.empty())
	{
		setIcon(utility::createButtonIcon(m_iconPath, m_buttonKey));
	}
}

void QtSelfRefreshIconButton::resizeEvent(QResizeEvent* event)
{
	if (m_autoElide)
	{
		m_updateTimer.stop();
		updateText(event->size().width());
	}

	QPushButton::resizeEvent(event);
}

void QtSelfRefreshIconButton::updateText(int width)
{
	if (!m_blockUpdate)
	{
		m_blockUpdate = true;
		QPushButton::setText(
			fontMetrics().elidedText(m_text, Qt::ElideLeft, width - iconSize().width() - 22));
		m_blockTimer.start(50);
	}
}
