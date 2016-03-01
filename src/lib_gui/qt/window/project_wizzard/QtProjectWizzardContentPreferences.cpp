#include "qt/window/project_wizzard/QtProjectWizzardContentPreferences.h"

#include <QSysInfo>

QtProjectWizzardContentPreferences::QtProjectWizzardContentPreferences(ProjectSettings* settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(settings, window)
	, m_headerSearch(nullptr)
	, m_frameworkSearch(nullptr)
{
	m_headerSearch = new QtProjectWizzardContentPathsHeaderSearchGlobal(settings, window);

	if (QSysInfo::macVersion() != QSysInfo::MV_None)
	{
		m_frameworkSearch = new QtProjectWizzardContentPathsFrameworkSearchGlobal(settings, window);
	}
}

void QtProjectWizzardContentPreferences::populateWindow(QGridLayout* layout)
{
	int row = 0;

	layout->setRowMinimumHeight(row, 10);
	row++;

	m_headerSearch->populateForm(layout, row);

	if (m_frameworkSearch)
	{
		m_frameworkSearch->populateForm(layout, row);
	}

	layout->setRowMinimumHeight(row, 10);

	layout->setRowStretch(row, 1);
}

void QtProjectWizzardContentPreferences::windowReady()
{
	m_window->updateTitle("PREFERENCES");
	m_window->updateNextButton("Save");
	m_window->setPreviousVisible(false);
}

void QtProjectWizzardContentPreferences::load()
{
	m_headerSearch->load();

	if (m_frameworkSearch)
	{
		m_frameworkSearch->load();
	}
}

void QtProjectWizzardContentPreferences::save()
{
	m_headerSearch->save();

	if (m_frameworkSearch)
	{
		m_frameworkSearch->save();
	}
}

bool QtProjectWizzardContentPreferences::isScrollAble() const
{
	return true;
}
