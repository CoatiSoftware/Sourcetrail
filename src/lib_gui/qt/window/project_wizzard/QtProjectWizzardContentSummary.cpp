#include "qt/window/project_wizzard/QtProjectWizzardContentSummary.h"

#include <QFormLayout>
#include <QSysInfo>

QtProjectWizzardContentSummary::QtProjectWizzardContentSummary(ProjectSettings* settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(settings, window)
	, m_data(nullptr)
	, m_buildFile(nullptr)
	, m_source(nullptr)
	, m_simple(nullptr)
	, m_headerSearch(nullptr)
	, m_frameworkSearch(nullptr)
{
	m_data = new QtProjectWizzardContentData(settings, window);
	m_buildFile = new QtProjectWizzardContentBuildFile(settings, window);
	m_source = new QtProjectWizzardContentPathsSource(settings, window);
	m_simple = new QtProjectWizzardContentSimple(settings, window);
	m_headerSearch = new QtProjectWizzardContentPathsHeaderSearch(settings, window);

	if (QSysInfo::macVersion() != QSysInfo::MV_None)
	{
		m_frameworkSearch = new QtProjectWizzardContentPathsFrameworkSearch(settings, window);
	}
}

QtProjectWizzardContentBuildFile* QtProjectWizzardContentSummary::contentBuildFile()
{
	return m_buildFile;
}

QtProjectWizzardContentPathsSource* QtProjectWizzardContentSummary::contentPathsSource()
{
	return m_source;
}

void QtProjectWizzardContentSummary::populateWindow(QWidget* widget)
{
	QFormLayout* layout = new QFormLayout();
	layout->setContentsMargins(10, 10, 10, 10);
	layout->setHorizontalSpacing(20);

	m_data->populateForm(layout);
	m_buildFile->populateForm(layout);
	m_source->populateForm(layout);
	m_simple->populateForm(layout);
	m_headerSearch->populateForm(layout);

	if (m_frameworkSearch)
	{
		m_frameworkSearch->populateForm(layout);
	}

	widget->setLayout(layout);
}

void QtProjectWizzardContentSummary::windowReady()
{
	m_window->updateTitle("NEW PROJECT - SUMMARY");
	m_window->updateDoneButton("Create");
}

void QtProjectWizzardContentSummary::load()
{
	m_data->load();
	m_buildFile->load();
	m_source->load();
	m_simple->load();
	m_headerSearch->load();

	if (m_frameworkSearch)
	{
		m_frameworkSearch->load();
	}
}

void QtProjectWizzardContentSummary::save()
{
	m_data->save();
	m_buildFile->save();
	m_source->save();
	m_simple->save();
	m_headerSearch->save();

	if (m_frameworkSearch)
	{
		m_frameworkSearch->save();
	}
}

bool QtProjectWizzardContentSummary::check()
{
	return
		m_data->check() &&
		m_buildFile->check() &&
		m_source->check() &&
		m_simple->check() &&
		m_headerSearch->check() &&
		(!m_frameworkSearch || m_frameworkSearch->check());
}
