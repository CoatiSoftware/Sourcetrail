#include "qt/window/project_wizzard/QtProjectWizzardContentPaths.h"

#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

#include "qt/element/QtDirectoryListBox.h"
#include "settings/ApplicationSettings.h"
#include "settings/CxxProjectSettings.h"
#include "settings/JavaProjectSettings.h"
#include "utility/file/FileManager.h"
#include "utility/file/FileSystem.h"
#include "utility/path_detector/cxx_header/CxxFrameworkPathDetector.h"
#include "utility/path_detector/cxx_header/CxxHeaderPathDetector.h"
#include "utility/path_detector/cxx_header/CxxVsHeaderPathDetector.h"
#include "utility/utility.h"

QtProjectWizzardContentPaths::QtProjectWizzardContentPaths(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(settings, window)
{
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

	m_list = new QtDirectoryListBox(this, m_titleString);
	layout->addWidget(m_list, row, QtProjectWizzardWindow::BACK_COL, 2, 1, Qt::AlignTop);

	row += 2;

	if (m_showFilesString.size() > 0)
	{
		layout->setRowStretch(row, 0);
		addFilesButton(m_showFilesString, layout, row);
	}

	if (m_pathDetector)
	{
		addDetection(layout, row);
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

	m_list = new QtDirectoryListBox(this, m_titleString);
	layout->addWidget(m_list, row, QtProjectWizzardWindow::BACK_COL);
	row++;

	if (m_showFilesString.size() > 0)
	{
		addFilesButton(m_showFilesString, layout, row);
		row++;
	}

	if (m_pathDetector)
	{
		addDetection(layout, row);
		row++;
	}
}

QSize QtProjectWizzardContentPaths::preferredWindowSize() const
{
	return QSize(750, 500);
}

bool QtProjectWizzardContentPaths::check()
{
	QString missingPaths;
	for (FilePath f : m_list->getList())
	{
		FilePath fi = f.expandEnvironmentVariables();
		if (!fi.isAbsolute())
		{
			fi = FilePath(m_settings->getProjectFileLocation()).concat(fi);
		}

		if (!fi.exists())
		{
			missingPaths.append(f.str().c_str());
			missingPaths.append("\n");
		}
	}

	if (!missingPaths.isEmpty())
	{
		QMessageBox msgBox;
		msgBox.setText("Some project paths do not exist.");
		msgBox.setDetailedText(missingPaths);
		msgBox.exec();
		return false;
	}

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

void QtProjectWizzardContentPaths::addDetection(QGridLayout* layout, int row)
{
	std::vector<std::string> detectorNames = m_pathDetector->getWorkingDetectorNames();
	if (!detectorNames.size())
	{
		return;
	}

	QLabel* label = new QLabel("Auto detection from:");

	m_detectorBox = new QComboBox();

	for (const std::string& detectorName: detectorNames)
	{
		m_detectorBox->addItem(detectorName.c_str());
	}

	QPushButton* button = new QPushButton("detect");
	button->setObjectName("windowButton");
	connect(button, SIGNAL(clicked()), this, SLOT(detectionClicked()));

	QHBoxLayout* hlayout = new QHBoxLayout();
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->addWidget(label);
	hlayout->addWidget(m_detectorBox);
	hlayout->addWidget(button);

	QWidget* detectionWidget = new QWidget();
	detectionWidget->setLayout(hlayout);

	layout->addWidget(detectionWidget, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft | Qt::AlignTop);
}

void QtProjectWizzardContentPaths::detectionClicked()
{
	std::vector<FilePath> paths = m_pathDetector->getPaths(m_detectorBox->currentText().toStdString());
	std::vector<FilePath> oldPaths = m_list->getList();
	m_list->setList(utility::unique(utility::concat(oldPaths, paths)));
}


QtProjectWizzardContentPathsSource::QtProjectWizzardContentPathsSource(
	std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window)
{
	m_showFilesString = "show files";

	setInfo(
		"Project Paths",
		"Add all directories or files you want to index. Usually these are all source and header files of "
		"your project or a subset of them.",
		"Project Paths define the files and directories that will be indexed by Coati. Usually these are the "
		"source and header files of your project or a subset of them."
	);
}

QSize QtProjectWizzardContentPathsSource::preferredWindowSize() const
{
	return QSize(750, 400);
}

void QtProjectWizzardContentPathsSource::load()
{
	m_list->setList(m_settings->getSourcePaths());
}

void QtProjectWizzardContentPathsSource::save()
{
	m_settings->setSourcePaths(m_list->getList());
}

std::vector<std::string> QtProjectWizzardContentPathsSource::getFileNames() const
{
	std::vector<FilePath> sourcePaths = m_settings->getAbsoluteSourcePaths();
	std::vector<FilePath> excludePaths = m_settings->getAbsoluteExcludePaths();
	std::vector<std::string> extensions = m_settings->getSourceExtensions();

	std::vector<FileInfo> fileInfos = FileSystem::getFileInfosFromPaths(sourcePaths, extensions);
	FilePath projectPath = m_settings->getProjectFileLocation();

	std::vector<std::string> list;
	for (const FileInfo& info : fileInfos)
	{
		FilePath path = info.path;

		bool excluded = false;
		for (FilePath p : excludePaths)
		{
			if (p == path || p.contains(path))
			{
				excluded = true;
				break;
			}
		}

		if (excluded)
		{
			continue;
		}

		if (projectPath.exists())
		{
			path = path.relativeTo(projectPath);
		}

		list.push_back(path.str());
	}

	return list;
}

QString QtProjectWizzardContentPathsSource::getFileNamesTitle() const
{
	return "Indexed Files";
}

QString QtProjectWizzardContentPathsSource::getFileNamesDescription() const
{
	return " files will be indexed.";
}

QtProjectWizzardContentPathsSourceJava::QtProjectWizzardContentPathsSourceJava(
	std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPathsSource(settings, window)
{
	setInfo(
		"Source Paths",
		"Add all directories or files you want to index. Usually these are all source files of "
		"your project or a subset of them.",
		"Source Paths define the files and directories that will be indexed by Coati. Usually these are the "
		"source files of your project or a subset of them."
	);
}

QtProjectWizzardContentPathsCDBHeader::QtProjectWizzardContentPathsCDBHeader(
	std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPathsSource(settings, window)
{
	m_showFilesString = "";

	setTitleString("Header Paths");
	setDescriptionString(
		"Where are the header files of the source files? Add the directories or files that should be "
		"indexed if included by one of the source files."
	);
	setHelpString(
		"The compilation database only contains source files. Add the header files or directories containing the header "
		"files here. Header files will be indexed if included."
	);
}


QtProjectWizzardContentPathsExclude::QtProjectWizzardContentPathsExclude(
	std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window)
{
	setInfo(
		"Exclude Paths",
		"Add all directories or files you want to exclude from indexing.",
		"Exclude Paths define the files and directories that will be left out from indexing."
	);
}

void QtProjectWizzardContentPathsExclude::load()
{
	m_list->setList(m_settings->getExcludePaths());
}

void QtProjectWizzardContentPathsExclude::save()
{
	m_settings->setExcludePaths(m_list->getList());
}


QtProjectWizzardContentPathsHeaderSearch::QtProjectWizzardContentPathsHeaderSearch(
	std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window)
{
	setInfo(
		"Include Paths",
		"Add the paths for resolving #include directives in the indexed source and header files.",
		"Include Paths define where additional files, that your project depends on, are found. Usually they are "
		"header files of frameworks or libraries that your project uses. These files won't be indexed, but Coati needs "
		"them for correct indexing."
	);
}

void QtProjectWizzardContentPathsHeaderSearch::load()
{
	std::shared_ptr<CxxProjectSettings> cxxSettings = std::dynamic_pointer_cast<CxxProjectSettings>(m_settings);
	if (cxxSettings)
	{
		m_list->setList(cxxSettings->getHeaderSearchPaths());
	}
}

void QtProjectWizzardContentPathsHeaderSearch::save()
{
	std::shared_ptr<CxxProjectSettings> cxxSettings = std::dynamic_pointer_cast<CxxProjectSettings>(m_settings);
	if (cxxSettings)
	{
		cxxSettings->setHeaderSearchPaths(m_list->getList());
	}
}

bool QtProjectWizzardContentPathsHeaderSearch::isScrollAble() const
{
	return true;
}

QtProjectWizzardContentPathsHeaderSearchGlobal::QtProjectWizzardContentPathsHeaderSearchGlobal(
	std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window)
{
	setInfo(
		"Global Include Paths",
		"These include paths will be used in all your projects. Use it to add system header paths "
		"(See <a href=\"https://coati.io/documentation/#FindingSystemHeaderLocations\">Finding System Header Locations</a> "
		"or use the auto detection below).",
		"Include Paths define where additional files, that your project depends on, are found. Usually they are "
		"header files of frameworks or libraries that your project uses. These files won't be indexed, but Coati needs "
		"them for correct indexing.\n\n"
		"Include Paths defined here will be used for all projects."
	);

	m_pathDetector = std::make_shared<CombinedPathDetector>();
	m_pathDetector->addDetector(std::make_shared<CxxHeaderPathDetector>("gcc"));
	m_pathDetector->addDetector(std::make_shared<CxxHeaderPathDetector>("clang"));
	m_pathDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(9, false));
	m_pathDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(9, true));
	m_pathDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(11, false));
	m_pathDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(11, true));
	m_pathDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(12, false));
	m_pathDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(12, true));
	m_pathDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(14, false));
	m_pathDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(14, true));
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
	std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window
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
	std::shared_ptr<CxxProjectSettings> cxxSettings = std::dynamic_pointer_cast<CxxProjectSettings>(m_settings);
	if (cxxSettings)
	{
		m_list->setList(cxxSettings->getFrameworkSearchPaths());
	}
}

void QtProjectWizzardContentPathsFrameworkSearch::save()
{
	std::shared_ptr<CxxProjectSettings> cxxSettings = std::dynamic_pointer_cast<CxxProjectSettings>(m_settings);
	if (cxxSettings)
	{
		cxxSettings->setFrameworkSearchPaths(m_list->getList());
	}
}

bool QtProjectWizzardContentPathsFrameworkSearch::isScrollAble() const
{
	return true;
}

QtProjectWizzardContentPathsFrameworkSearchGlobal::QtProjectWizzardContentPathsFrameworkSearchGlobal(
	std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window)
{
	setInfo(
		"Global Framework Search Paths",
		"These framework search paths will be used in all your projects. Use it to add system frameworks "
		"(See <a href=\"https://coati.io/documentation/#FindingSystemHeaderLocations\">"
		"Finding System Header Locations</a> or use the auto detection below).",
		"Framework Search Paths define where MacOS framework containers (.framework), that your project depends on, are "
		"found.\n\n"
		"Framework Search Paths defined here will be used for all projects."
	);

	m_pathDetector = std::make_shared<CombinedPathDetector>();
	m_pathDetector->addDetector(std::make_shared<CxxFrameworkPathDetector>("gcc"));
	m_pathDetector->addDetector(std::make_shared<CxxFrameworkPathDetector>("clang"));
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


QtProjectWizzardContentPathsClassJava::QtProjectWizzardContentPathsClassJava(
	std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window)
{
	setInfo(
		"Class Path",
		"Paths to .jar files and root directories of .class files your project depends on.",
		"Enter the paths to .jar files and root directories of .class files your project depends on."
	);
}

void QtProjectWizzardContentPathsClassJava::load()
{
	std::shared_ptr<JavaProjectSettings> javaSettings = std::dynamic_pointer_cast<JavaProjectSettings>(m_settings);
	if (javaSettings)
	{
		m_list->setList(javaSettings->getClasspaths());
	}
}

void QtProjectWizzardContentPathsClassJava::save()
{
	std::shared_ptr<JavaProjectSettings> javaSettings = std::dynamic_pointer_cast<JavaProjectSettings>(m_settings);
	if (javaSettings)
	{
		javaSettings->setClasspaths(m_list->getList());
	}
}
