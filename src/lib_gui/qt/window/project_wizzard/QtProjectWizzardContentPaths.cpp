#include "qt/window/project_wizzard/QtProjectWizzardContentPaths.h"

#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QComboBox>

#include "qt/element/QtDirectoryListBox.h"
#include "qt/window/QtSelectPathsDialog.h"
#include "settings/ApplicationSettings.h"
#include "settings/CxxProjectSettings.h"
#include "settings/JavaProjectSettings.h"
#include "utility/CompilationDatabase.h"
#include "utility/file/FileManager.h"
#include "utility/file/FileSystem.h"
#include "utility/utility.h"
#include "utility/utilityPathDetection.h"

QtProjectWizzardContentPaths::QtProjectWizzardContentPaths(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(settings, window)
	, m_makePathsRelativeToProjectFileLocation(true)
{
}

void QtProjectWizzardContentPaths::populate(QGridLayout* layout, int& row)
{
	QLabel* label = createFormLabel(m_titleString);
	layout->addWidget(label, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);

	if (m_helpString.size() > 0)
	{
		addHelpButton(m_helpString, layout, row);
	}

	m_list = new QtDirectoryListBox(this, m_titleString);

	if (m_makePathsRelativeToProjectFileLocation)
	{
		m_list->setRelativeRootDirectory(m_settings->getProjectFileLocation());
	}

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

bool QtProjectWizzardContentPaths::check()
{
	QString missingPaths;

	for (const FilePath& path : m_list->getList())
	{
		std::vector<FilePath> expandedPaths = path.expandEnvironmentVariables();
		for (FilePath expandedPath: path.expandEnvironmentVariables())
		{
			if (m_settings)
			{
				expandedPath = m_settings->makePathAbsolute(expandedPath);
			}

			if (!expandedPath.exists())
			{
				missingPaths.append(expandedPath.str().c_str());
				missingPaths.append("\n");
				break;
			}
		}
	}

	if (!missingPaths.isEmpty())
	{
		QMessageBox msgBox;
		msgBox.setText("Some provided paths do not exist.");
		msgBox.setDetailedText(missingPaths);
		msgBox.exec();
		return false;
	}

	return true;
}

void QtProjectWizzardContentPaths::setTitleString(const QString& title)
{
	m_titleString = title;
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

	setTitleString("Indexed Paths");
	setHelpString(
		"Indexed Paths define the files and directories that will be indexed by Coati. Provide a directory to recursively "
		"add all contained files.<br />"
		"<br />"
		"If your project's source code resides in one location, but generated source files are kept at a different location, "
		"you will also need to add that directory.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}."
	);
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

QtProjectWizzardContentPathsCDBHeader::QtProjectWizzardContentPathsCDBHeader(
	std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPathsSource(settings, window)
{
	m_showFilesString = "";

	setTitleString("Indexed Header Paths");
	setHelpString(
		"Define which header files should be indexed by Coati. Provide a directory to recursively add all contained files. "
		"Every time an included header is encountered, Coati will check if the file is part of the indexed headers to "
		"decide whether or not to index it.<br />"
		"<br />"
		"Just enter the root path of your project if you want Coati to index all contained headers it encounters.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}."
	);
}

void QtProjectWizzardContentPathsCDBHeader::populate( QGridLayout* layout, int& row)
{
	QtProjectWizzardContentPaths::populate(layout, row);

	QPushButton* button = new QPushButton("Select from Include Paths");
	button->setObjectName("windowButton");
	connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));

	layout->addWidget(button, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignRight | Qt::AlignTop);
	row++;
}

bool QtProjectWizzardContentPathsCDBHeader::check()
{
	if (!m_list->getList().size())
	{
		QMessageBox msgBox;
		msgBox.setText("You didn't specify any Indexed Header Paths.");
		msgBox.setInformativeText(
			"Coati will only index the source files listed in the compilation database file and none of the included "
			"header files.");
		msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Ok);
		int ret = msgBox.exec();

		return ret == QMessageBox::Ok;
	}
	else
	{
		return QtProjectWizzardContentPaths::check();
	}
}

void QtProjectWizzardContentPathsCDBHeader::buttonClicked()
{
	save();

	if (!m_filesDialog)
	{
		std::vector<std::string> fileNames = getFileNames();

		m_filesDialog = std::make_shared<QtSelectPathsDialog>(
			"Select from Include Paths",
			"The list contains all Include Paths found in the Compilation Database. Red paths do not exist. Select the "
			"paths containing the header files you want to index with Coati.");
		m_filesDialog->setup();


		utility::CompilationDatabase cdb(
			dynamic_cast<CxxProjectSettings*>(m_settings.get())->getAbsoluteCompilationDatabasePath().str());

		std::vector<FilePath> cdbHeaderPaths = cdb.getAllHeaderPaths();
		std::vector<FilePath> sourcePaths = m_settings->getSourcePaths();

		cdbHeaderPaths = utility::unique(utility::concat(sourcePaths, cdbHeaderPaths));

		dynamic_cast<QtSelectPathsDialog*>(m_filesDialog.get())->setPathsList(cdbHeaderPaths, sourcePaths);

		connect(m_filesDialog.get(), SIGNAL(finished()), this, SLOT(savedFilesDialog()));
		connect(m_filesDialog.get(), SIGNAL(canceled()), this, SLOT(closedFilesDialog()));
	}

	m_filesDialog->showWindow();
	m_filesDialog->raise();
}

void QtProjectWizzardContentPathsCDBHeader::savedFilesDialog()
{
	// TODO: extend instead of replace
	m_list->setList(dynamic_cast<QtSelectPathsDialog*>(m_filesDialog.get())->getPathsList());

	closedFilesDialog();
}

QtProjectWizzardContentPathsExclude::QtProjectWizzardContentPathsExclude(
	std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window)
{
	setTitleString("Exclude Paths");
	setHelpString(
		"Exclude Paths define the files and directories that will be left out from indexing.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}.");
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
	std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window, bool isCDB
)
	: QtProjectWizzardContentPaths(settings, window)
{
	setTitleString(isCDB ? "Additional Include Paths" : "Include Paths");
	setHelpString(
		((isCDB ? "<b>Note</b>: Use the Additional Include Paths to add paths that are missin in the CDB.<br /><br />" : "") + std::string(
		"Include Paths are used for resolving #include directives in the indexed source and header files. These paths are "
		"usually passed to the compiler with the '-I' or '-iquote' flags.<br />"
		"<br />"
		"Add all paths #include directives throughout your project are relative to. If all #include directives are "
		"specified relative to the project's root directory, please add that one.<br />"
		"<br />"
		"If your project also includes files from external libraries (e.g. boost), please add these directories as well "
		"(e.g. add '&lt;boost_home&gt;/include').<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}.")).c_str()
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
	setTitleString("Global Include Paths");
	setHelpString(
		"The Global Include Paths will be used in all your projects - in addition to the project specific Include Paths. "
		"These paths are usually passed to the compiler with the '-isystem' flag.<br />"
		"<br />"
		"Use them to add system header paths (See <a href=\"https://coati.io/documentation/#FindingSystemHeaderLocations\">"
		"Finding System Header Locations</a> or use the auto detection below)."
	);

	m_pathDetector = utility::getCxxHeaderPathDetector();
	m_makePathsRelativeToProjectFileLocation = false;
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
	std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window, bool isCDB
)
	: QtProjectWizzardContentPaths(settings, window)
{
	setTitleString(isCDB ? "Additional Framework Search Paths" : "Framework Search Paths");
	setHelpString(
		"Framework Search Paths define where MacOS framework containers (.framework), that your project depends on, are "
		"found. These paths are usually passed to the compiler with the '-iframework' flag.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}."
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
	setTitleString("Global Framework Search Paths");
	setHelpString(
		"The Global Framework Search Paths will be used in all your projects - in addition to the project specific "
		"Framework Search Paths.<br />"
		"<br />"
		"They define where MacOS framework containers (.framework) are found "
		"(See <a href=\"https://coati.io/documentation/#FindingSystemHeaderLocations\">"
		"Finding System Header Locations</a> or use the auto detection below)."
	);

	m_pathDetector = utility::getCxxFrameworkPathDetector();
	m_makePathsRelativeToProjectFileLocation = false;
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
	setTitleString("Class Path");
	setHelpString(
		"Enter all the .jar files your project depends on. If your project depends on uncompiled java code that should "
		"not be indexed, please add the root directory of those .java files here (the one where all the package names are relative to).<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}."
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
