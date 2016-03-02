#include "qt/window/project_wizzard/QtProjectWizzardContentSummary.h"

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

	m_compilerFlags = new QtProjectWizzardContentFlags(settings, window);
}

QtProjectWizzardContentBuildFile* QtProjectWizzardContentSummary::contentBuildFile()
{
	return m_buildFile;
}

QtProjectWizzardContentPathsSource* QtProjectWizzardContentSummary::contentPathsSource()
{
	return m_source;
}

void QtProjectWizzardContentSummary::populateWindow(QGridLayout* layout)
{
	int row = 0;

	layout->setRowMinimumHeight(row, 10);
	row++;

	m_data->populateForm(layout, row);
	layout->setRowMinimumHeight(row, 15);
	row++;

	int row2 = row;
	m_buildFile->populateForm(layout, row);
	if (row != row2)
	{
		layout->setRowMinimumHeight(row, 15);
		row++;
	}

	m_source->populateForm(layout, row);
	layout->setRowMinimumHeight(row, 15);
	row++;

	m_simple->populateForm(layout, row);
	m_headerSearch->populateForm(layout, row);

	if (m_frameworkSearch)
	{
		layout->setRowMinimumHeight(row, 15);
		row++;
		m_frameworkSearch->populateForm(layout, row);
	}

	layout->setRowMinimumHeight(row, 15);
	row++;

	QFrame* separator = new QFrame();
	separator->setFrameShape(QFrame::HLine);

	QPalette palette = separator->palette();
	palette.setColor(QPalette::WindowText, Qt::lightGray);
	separator->setPalette(palette);

	layout->addWidget(separator, row, 0, 1, -1);
	row++;

	QLabel* advancedLabel = createFormLabel("ADVANCED");
	layout->addWidget(advancedLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);
	row++;

	m_compilerFlags->populateForm(layout, row);

	layout->setRowMinimumHeight(row, 10);
}

void QtProjectWizzardContentSummary::windowReady()
{
	m_window->updateTitle("NEW PROJECT - SUMMARY");
	m_window->updateNextButton("Create");
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

	m_compilerFlags->load();
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

	m_compilerFlags->save();
}

bool QtProjectWizzardContentSummary::check()
{
	return
		m_data->check() &&
		m_buildFile->check() &&
		m_source->check() &&
		m_simple->check() &&
		m_headerSearch->check() &&
		(!m_frameworkSearch || m_frameworkSearch->check()) &&
		m_compilerFlags->check();
}

bool QtProjectWizzardContentSummary::isScrollAble() const
{
	return true;
}
