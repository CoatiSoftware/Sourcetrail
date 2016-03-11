#include "qt/window/project_wizzard/QtProjectWizzardContentPaths.h"

#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

#include "qt/element/QtDirectoryListBox.h"
#include "settings/ApplicationSettings.h"
#include "utility/file/FileSystem.h"
#include "utility/utility.h"

QtProjectWizzardContentPaths::QtProjectWizzardContentPaths(ProjectSettings* settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(settings, window)
{
}

void QtProjectWizzardContentPaths::populateWindow(QGridLayout* layout)
{
	int row = 0;
	populateWindow(layout, row);
}

void QtProjectWizzardContentPaths::populateWindow(QGridLayout* layout, int& row)
{
	layout->setRowMinimumHeight(row++, 10);

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

	if (m_showFilesString.size() > 0)
	{
		layout->setRowStretch(row, 10);
		addFilesButton(m_showFilesString, layout, row);
	}

	row++;

	layout->setColumnStretch(QtProjectWizzardWindow::FRONT_COL, 1);
	layout->setColumnStretch(QtProjectWizzardWindow::BACK_COL, 2);
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

	if (m_showFilesString.size() > 0)
	{
		addFilesButton(m_showFilesString, layout, row);
		row++;
	}
}

QSize QtProjectWizzardContentPaths::preferredWindowSize() const
{
	return QSize(750, 500);
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

QtProjectWizzardContentPathsSource::QtProjectWizzardContentPathsSource(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window)
{
	m_showFilesString = "show files";

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
	return QSize(750, 370);
}

void QtProjectWizzardContentPathsSource::load()
{
	m_list->setList(m_settings->getSourcePaths());
}

void QtProjectWizzardContentPathsSource::save()
{
	m_settings->setSourcePaths(m_list->getList());
}

bool QtProjectWizzardContentPathsSource::check()
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

QStringList QtProjectWizzardContentPathsSource::getFileNames() const
{
	return getSourceFileNames(false);
}

QString QtProjectWizzardContentPathsSource::getFileNamesTitle() const
{
	return "Analyzed Files";
}

QString QtProjectWizzardContentPathsSource::getFileNamesDescription() const
{
	return "files will be analyzed.";
}

QStringList QtProjectWizzardContentPathsSource::getSourceFileNames(bool headersOnly) const
{
	std::vector<FilePath> sourcePaths = m_settings->getSourcePaths();

	std::vector<std::string> extensions;
	if (!headersOnly)
	{
		utility::append(extensions, m_settings->getSourceExtensions());
	}
	utility::append(extensions, m_settings->getHeaderExtensions());

	std::vector<FileInfo> fileInfos = FileSystem::getFileInfosFromPaths(sourcePaths, extensions);

	FilePath projectPath = FilePath(m_settings->getProjectFileLocation());

	QStringList list;
	for (const FileInfo& info : fileInfos)
	{
		FilePath path = info.path;

		if (projectPath.exists())
		{
			path = path.relativeTo(projectPath);
		}

		list << QString::fromStdString(path.str());
	}

	return list;
}

QtProjectWizzardContentPathsSourceSimple::QtProjectWizzardContentPathsSourceSimple(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPathsSource(settings, window)
{
	m_showFilesString = "show files";

	setTitleString("Project Paths");
	setDescriptionString(
		"Add all directories or files you want to analyse with Coati. It is sufficient to just provide the top level "
		"project directory."
	);
}

QtProjectWizzardContentPathsCDBHeader::QtProjectWizzardContentPathsCDBHeader(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPathsSource(settings, window)
{
	m_showFilesString = "show header files";

	setTitleString("Header Paths");
	setDescriptionString(
		"Add the header files or directories containing the header files of the source files above. These header files "
		"will be analyzed if included."
	);
	setHelpString(
		"The compilation database only contains source files. Add the header files or directories containing the header "
		"files of these source files. The header files will be analyzed if included."
	);
}

bool QtProjectWizzardContentPathsCDBHeader::check()
{
	return true;
}

QStringList QtProjectWizzardContentPathsCDBHeader::getFileNames() const
{
	return getSourceFileNames(true);
}

QString QtProjectWizzardContentPathsCDBHeader::getFileNamesTitle() const
{
	return "Header Files";
}

QString QtProjectWizzardContentPathsCDBHeader::getFileNamesDescription() const
{
	return "header files found.";
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
}

void QtProjectWizzardContentPathsHeaderSearch::load()
{
	m_list->setList(m_settings->getHeaderSearchPaths());
}

void QtProjectWizzardContentPathsHeaderSearch::save()
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
		"header paths (See <a href=\"https://coati.io/documentation/#FindingSystemHeaderLocations\">Finding "
		"System Header Locations</a>).",
		"Header Search Paths define where additional headers, that your project depends on, are found. Usually they are "
		"header files of frameworks or libraries that your project uses. These files won't be analyzed, but Coati needs "
		"them for correct analysis.\n\n"
		"Header Search Paths defined here will be used for all projects."
	);
}

void QtProjectWizzardContentPathsHeaderSearchGlobal::load()
{
	m_list->setList(ApplicationSettings::getInstance()->getHeaderSearchPaths());
}

void QtProjectWizzardContentPathsHeaderSearchGlobal::save()
{
	ApplicationSettings::getInstance()->setHeaderSearchPaths(m_list->getList());
	ApplicationSettings::getInstance()->save();
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
}

void QtProjectWizzardContentPathsFrameworkSearch::load()
{
	m_list->setList(m_settings->getFrameworkSearchPaths());
}

void QtProjectWizzardContentPathsFrameworkSearch::save()
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
		"(See <a href=\"https://coati.io/documentation/#FindingSystemHeaderLocations\">"
		"Finding System Header Locations</a>).",
		"Framework Search Paths define where MacOS framework containers (.framework), that your project depends on, are "
		"found.\n\n"
		"Framework Search Paths defined here will be used for all projects."
	);
}

void QtProjectWizzardContentPathsFrameworkSearchGlobal::load()
{
	m_list->setList(ApplicationSettings::getInstance()->getFrameworkSearchPaths());
}

void QtProjectWizzardContentPathsFrameworkSearchGlobal::save()
{
	ApplicationSettings::getInstance()->setFrameworkSearchPaths(m_list->getList());
	ApplicationSettings::getInstance()->save();
}
