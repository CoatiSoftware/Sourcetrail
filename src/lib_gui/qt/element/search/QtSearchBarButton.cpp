#include "QtSearchBarButton.h"

#include "ApplicationSettings.h"

QtSearchBarButton::QtSearchBarButton(const FilePath& iconPath, bool small, QWidget* parent)
	: QtSelfRefreshIconButton(QLatin1String(""), iconPath, "search/button", parent), m_small(small)
{
	refresh();
}

void QtSearchBarButton::refresh()
{
	QtSelfRefreshIconButton::refresh();

	int size = m_small ? 10 : 16;

	float height = std::max(ApplicationSettings::getInstance()->getFontSize() + size, size + 14);
	setFixedHeight(height);

	if (!m_small)
	{
		int iconSize = int(height / 4) * 2 + 2;
		setIconSize(QSize(iconSize, iconSize));
	}
}
