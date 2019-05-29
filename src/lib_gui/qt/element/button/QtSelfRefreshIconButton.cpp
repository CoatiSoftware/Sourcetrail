#include "QtSelfRefreshIconButton.h"

#include "utilityQt.h"

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
