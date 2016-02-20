#include "qt/window/project_wizzard/QtProjectWizzardContentPaths.h"

#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

#include "qt/element/QtDirectoryListBox.h"
#include "settings/ApplicationSettings.h"


QtProjectWizzardContentPaths::QtProjectWizzardContentPaths(ProjectSettings* settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(settings, window)
	, m_subPaths(nullptr)
	, m_addShowSourcesButton(false)
{
}

void QtProjectWizzardContentPaths::populateWindow(QGridLayout* layout)
{
	int row = 0;

	layout->setRowMinimumHeight(row, 10);
	row++;

	populateLayout(layout, row);

	if (m_addShowSourcesButton)
	{
		layout->setRowStretch(row, 10);
		addSourcesButton(layout, row);
	}

	if (m_subPaths)
	{
		layout->setRowMinimumHeight(row, 30);
		row++;

		m_subPaths->populateLayout(layout, row);

		layout->setRowMinimumHeight(row, 10);
		row++;
	}

	layout->setColumnStretch(QtProjectWizzardWindow::FRONT_COL, 1);
	layout->setColumnStretch(QtProjectWizzardWindow::BACK_COL, 2);
}

void QtProjectWizzardContentPaths::populateLayout(QGridLayout* layout, int& row)
{
	QLabel* title = new QLabel(m_titleString);
	title->setWordWrap(true);
	title->setObjectName("section");
	layout->addWidget(title, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);
	layout->setRowStretch(row, 0);

	QLabel* text = new QLabel(m_descriptionString);
	text->setWordWrap(true);
	text->setOpenExternalLinks(true);
	layout->addWidget(text, row + 1, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);
	layout->setRowStretch(row + 1, 1);

	m_list = new QtDirectoryListBox(this);
	layout->addWidget(m_list, row, QtProjectWizzardWindow::BACK_COL, 2, 1, Qt::AlignTop);

	row += 2;
}

void QtProjectWizzardContentPaths::populateForm(QGridLayout* layout, int& row)
{
	QLabel* label = createFormLabel(m_titleString);
	layout->addWidget(label, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);

	if (m_helpString.size() > 0)
	{
		addHelpButton(m_helpString, layout, row);
	}

	m_list = new QtDirectoryListBox(this);
	layout->addWidget(m_list, row, QtProjectWizzardWindow::BACK_COL);
	row++;

	if (m_addShowSourcesButton)
	{
		addSourcesButton(layout, row);
	}

	if (m_subPaths)
	{
		m_subPaths->populateForm(layout, row);
	}
}

void QtProjectWizzardContentPaths::load()
{
	loadPaths();

	if (m_subPaths)
	{
		return m_subPaths->loadPaths();
	}
}

void QtProjectWizzardContentPaths::save()
{
	savePaths();

	if (m_subPaths)
	{
		return m_subPaths->savePaths();
	}
}

bool QtProjectWizzardContentPaths::check()
{
	if (m_subPaths)
	{
		return checkPaths() && m_subPaths->checkPaths();
	}

	return checkPaths();
}

QSize QtProjectWizzardContentPaths::preferredWindowSize() const
{
	return QSize(850, 500);
}

void QtProjectWizzardContentPaths::loadPaths()
{
}

void QtProjectWizzardContentPaths::savePaths()
{
}

bool QtProjectWizzardContentPaths::checkPaths()
{
	return true;
}

void QtProjectWizzardContentPaths::setInfo(const QString& title, const QString& description, const QString& help)
{
	m_titleString = title;
	m_descriptionString = description;
	m_helpString = help;
}

void QtProjectWizzardContentPaths::setTitleString(const QString& title)
{
	m_titleString = title;
}

void QtProjectWizzardContentPaths::setDescriptionString(const QString& description)
{
	m_descriptionString = description;
}

void QtProjectWizzardContentPaths::setHelpString(const QString& help)
{
	m_helpString = help;
}

void QtProjectWizzardContentPaths::showSourcesClicked()
{
	emit showSourceFiles();
}

void QtProjectWizzardContentPaths::addSourcesButton(QGridLayout* layout, int& row)
{
	if (!m_addShowSourcesButton)
	{
		return;
	}

	QPushButton* button = new QPushButton("show files");
	button->setObjectName("windowButton");
	layout->addWidget(button, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignRight | Qt::AlignTop);
	connect(button, SIGNAL(clicked()), this, SLOT(showSourcesClicked()));
	row++;
}

QtProjectWizzardContentPathsSource::QtProjectWizzardContentPathsSource(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window)
{
	m_addShowSourcesButton = true;

	setInfo(
		"Project Paths",
		"Add all directories or files you want to analyse. Usually these are all source and header files of "
		"your project or a subset of them.",
		"Project Paths define the source files and directories that will be analyzed by Coati. Usually these are the "
		"source and header files of your project or a subset of them."
	);
}

QSize QtProjectWizzardContentPathsSource::preferredWindowSize() const
{
	return QSize(850, 370);
}

void QtProjectWizzardContentPathsSource::loadPaths()
{
	m_list->setList(m_settings->getSourcePaths());
}

void QtProjectWizzardContentPathsSource::savePaths()
{
	m_settings->setSourcePaths(m_list->getList());
}

bool QtProjectWizzardContentPathsSource::checkPaths()
{
	if (m_list->getList().size() == 0)
	{
		QMessageBox msgBox;
		msgBox.setText("Please add at least one path.");
		msgBox.exec();
		return false;
	}

	return true;
}

QtProjectWizzardContentPathsSourceSimple::QtProjectWizzardContentPathsSourceSimple(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPathsSource(settings, window)
{
	m_addShowSourcesButton = true;

	setTitleString("Project Paths");
	setDescriptionString(
		"Add all directories or files you want to analyse with Coati. It is sufficient to just provide the top level "
		"project directory."
	);
}


QtProjectWizzardContentPathsHeaderSearch::QtProjectWizzardContentPathsHeaderSearch(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window)
{
	setInfo(
		"Header Search Paths",
		"Add the header search paths for resolving #include directives in the analyzed source and header files.",
		"Header Search Paths define where additional headers, that your project depends on, are found. Usually they are "
		"header files of frameworks or libraries that your project uses. These files won't be analyzed, but Coati needs "
		"them for correct analysis."
	);

	m_subPaths = new QtProjectWizzardContentPathsHeaderSearchGlobal(settings, window);
}

void QtProjectWizzardContentPathsHeaderSearch::loadPaths()
{
	m_list->setList(m_settings->getHeaderSearchPaths());
}

void QtProjectWizzardContentPathsHeaderSearch::savePaths()
{
	m_settings->setHeaderSearchPaths(m_list->getList());
}

bool QtProjectWizzardContentPathsHeaderSearch::isScrollAble() const
{
	return true;
}

QtProjectWizzardContentPathsHeaderSearchSimple::QtProjectWizzardContentPathsHeaderSearchSimple(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPathsHeaderSearch(settings, window)
{
	setTitleString("External Header Search Paths");
	setDescriptionString(
		"Add the header search paths to external dependencies used in your project. The header search paths are "
		"needed to resolve #include directives within your source and header files."
	);
}

QtProjectWizzardContentPathsHeaderSearchGlobal::QtProjectWizzardContentPathsHeaderSearchGlobal(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window)
{
	setInfo(
		"Global Header Search Paths",
		"These header search paths will be used in all your projects. Use it to add system header and Standard Library "
		"header paths (See <a href=\"https://staging.coati.io/documentation/#FindingSystemHeaderLocations\">Finding "
		"System Header Locations</a>).",
		"Header Search Paths define where additional headers, that your project depends on, are found. Usually they are "
		"header files of frameworks or libraries that your project uses. These files won't be analyzed, but Coati needs "
		"them for correct analysis.\n\n"
		"Header Search Paths defined here will be used for all projects."
	);
}

void QtProjectWizzardContentPathsHeaderSearchGlobal::loadPaths()
{
	m_list->setList(ApplicationSettings::getInstance()->getHeaderSearchPaths());
}

void QtProjectWizzardContentPathsHeaderSearchGlobal::savePaths()
{
	ApplicationSettings::getInstance()->setHeaderSearchPaths(m_list->getList());
}


QtProjectWizzardContentPathsFrameworkSearch::QtProjectWizzardContentPathsFrameworkSearch(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window)
{
	setInfo(
		"Framework Search Paths",
		"Add search paths to Mac OS framework containers (.framework) that the project depends on.",
		"Framework Search Paths define where MacOS framework containers (.framework), that your project depends on, are "
		"found."
	);

	m_subPaths = new QtProjectWizzardContentPathsFrameworkSearchGlobal(settings, window);
}

void QtProjectWizzardContentPathsFrameworkSearch::loadPaths()
{
	m_list->setList(m_settings->getFrameworkSearchPaths());
}

void QtProjectWizzardContentPathsFrameworkSearch::savePaths()
{
	m_settings->setFrameworkSearchPaths(m_list->getList());
}

bool QtProjectWizzardContentPathsFrameworkSearch::isScrollAble() const
{
	return true;
}

QtProjectWizzardContentPathsFrameworkSearchGlobal::QtProjectWizzardContentPathsFrameworkSearchGlobal(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window)
{
	setInfo(
		"Global Framework Search Paths",
		"These framework search paths will be used in all your projects. Use it to add system frameworks "
		"(See <a href=\"https://staging.coati.io/documentation/#FindingSystemHeaderLocations\">"
		"Finding System Header Locations</a>).",
		"Framework Search Paths define where MacOS framework containers (.framework), that your project depends on, are "
		"found.\n\n"
		"Framework Search Paths defined here will be used for all projects."
	);
}

void QtProjectWizzardContentPathsFrameworkSearchGlobal::loadPaths()
{
	m_list->setList(ApplicationSettings::getInstance()->getFrameworkSearchPaths());
}

void QtProjectWizzardContentPathsFrameworkSearchGlobal::savePaths()
{
	ApplicationSettings::getInstance()->setFrameworkSearchPaths(m_list->getList());
}
