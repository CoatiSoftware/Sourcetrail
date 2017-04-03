#include "qt/window/project_wizzard/QtProjectWizzard.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSysInfo>

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentCDBSource.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentData.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentExtensions.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentFlags.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPath.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPaths.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPreferences.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSimple.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSummary.h"
#include "qt/window/project_wizzard/QtProjectWizzardWindow.h"
#include "settings/SourceGroupSettingsCxx.h"
#include "settings/SourceGroupSettingsJava.h"
#include "utility/file/FileSystem.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessagePluginPortChange.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageScrollSpeedChange.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/utility.h"
#include "utility/utilityPathDetection.h"
#include "utility/utilityString.h"
#include "utility/UUIDUtility.h"

#include "Application.h"

QtProjectWizzard::QtProjectWizzard(QWidget* parent)
	: QtWindowStackElement(parent)
	, m_windowStack(this)
	, m_editing(false)
{
	connect(&m_windowStack, SIGNAL(push()), this, SLOT(windowStackChanged()));
	connect(&m_windowStack, SIGNAL(pop()), this, SLOT(windowStackChanged()));

	// save old application settings so they can be compared later
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
	m_appSettings.setHeaderSearchPaths(appSettings->getHeaderSearchPaths());
	m_appSettings.setFrameworkSearchPaths(appSettings->getFrameworkSearchPaths());
	m_appSettings.setScrollSpeed(appSettings->getScrollSpeed());
	m_appSettings.setCoatiPort(appSettings->getCoatiPort());
	m_appSettings.setPluginPort(appSettings->getPluginPort());
}

void QtProjectWizzard::showWindow()
{
	QtWindowStackElement* element = m_windowStack.getTopWindow();

	if (element)
	{
		element->show();
	}
}

void QtProjectWizzard::hideWindow()
{
	QtWindowStackElement* element = m_windowStack.getTopWindow();

	if (element)
	{
		element->hide();
	}
}

void QtProjectWizzard::newProject()
{
	QtProjectWizzardWindow* window = createWindowWithContent(
		[this](QtProjectWizzardWindow* window)
		{
			return new QtProjectWizzardContentSelect(window);
		}
	);
	window->setPreferredSize(QSize(570, 380));

	connect(dynamic_cast<QtProjectWizzardContentSelect*>(window->content()),
		SIGNAL(selected(SourceGroupType)),
		this, SLOT(selectedProjectType(SourceGroupType)));

	window->setNextEnabled(false);
	window->setPreviousEnabled(false);
	window->updateSubTitle("Type Selection");
}

void QtProjectWizzard::newProjectFromSolution(const std::string& ideId, const FilePath& solutionPath)
{
	int i = 0;
}

void QtProjectWizzard::newProjectFromCDB(const FilePath& filePath, const std::vector<FilePath>& headerPaths)
{
	m_projectSettings = std::make_shared<ProjectSettings>();

	std::shared_ptr<SourceGroupSettingsCxx> sourceGroupSettings = std::make_shared<SourceGroupSettingsCxx>(UUIDUtility::getUUIDString(), SOURCE_GROUP_CXX_CDB, m_projectSettings.get());
	m_projectSettings->setProjectName(filePath.withoutExtension().fileName());
	m_projectSettings->setProjectFileLocation(filePath.parentDirectory());
	sourceGroupSettings->setCompilationDatabasePath(filePath);
	sourceGroupSettings->setSourcePaths(headerPaths);
	m_sourceGroupSettings = sourceGroupSettings;

	emptyProjectCDBVS();
}

void QtProjectWizzard::refreshProjectFromSolution(const std::string& ideId, const std::string& solutionPath)
{
	int i = 0;
}

void QtProjectWizzard::editProject(const FilePath& settingsPath)
{
	std::shared_ptr<ProjectSettings> settings = std::make_shared<ProjectSettings>(settingsPath);
	settings->reload();
	editProject(settings);
}

void QtProjectWizzard::editProject(std::shared_ptr<ProjectSettings> settings)
{
	m_projectSettings = settings;

	{
		const std::vector<std::shared_ptr<SourceGroupSettings>> allSourceGroupSettings = m_projectSettings->getAllSourceGroupSettings();
		if (!allSourceGroupSettings.empty())
		{
			// Todo: regard the rest of the source groups once we can have more than one in a project.
			m_sourceGroupSettings = allSourceGroupSettings.front();
		}
	}
	m_editing = true;

	switch (getLanguageTypeForSourceGroupType(m_sourceGroupSettings->getType()))
	{
		case LANGUAGE_JAVA:
			showSummaryJava();
			break;

		case LANGUAGE_C:
		case LANGUAGE_CPP:
			showSummary();
			break;

		default:
			break;
	}
}

void QtProjectWizzard::showPreferences()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->setIsForm(true);

			summary->addContent(new QtProjectWizzardContentPreferences(window));

			summary->addContent(new QtProjectWizzardContentPathsHeaderSearchGlobal(window));
			if (QSysInfo::macVersion() != QSysInfo::MV_None)
			{
				summary->addContent(new QtProjectWizzardContentPathsFrameworkSearchGlobal(window));
			}

			window->setup();

			window->updateTitle("PREFERENCES");
			window->updateNextButton("Save");
			window->setPreviousVisible(false);
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(savePreferences()));
}

bool QtProjectWizzard::applicationSettingsContainVisualStudioHeaderSearchPaths()
{
	std::vector<FilePath> expandedPaths;
	const std::shared_ptr<CombinedPathDetector> headerPathDetector = utility::getCxxVsHeaderPathDetector();
	for (const std::string& detectorName: headerPathDetector->getWorkingDetectorNames())
	{
		for (const FilePath& path: headerPathDetector->getPaths(detectorName))
		{
			utility::append(expandedPaths, path.expandEnvironmentVariables());
		}
	}

	std::vector<FilePath> usedExpandedGlobalHeaderSearchPaths = ApplicationSettings::getInstance()->getHeaderSearchPathsExpanded();
	for (const FilePath& usedExpandedPath: usedExpandedGlobalHeaderSearchPaths)
	{
		for (const FilePath& expandedPath: expandedPaths)
		{
			if (expandedPath == usedExpandedPath)
			{
				return true;
			}
		}
	}

	return false;
}

QtProjectWizzardWindow* QtProjectWizzard::createWindowWithContent(
	std::function<QtProjectWizzardContent*(QtProjectWizzardWindow*)> func
){
	QtProjectWizzardWindow* window = new QtProjectWizzardWindow(parentWidget());

	connect(window, SIGNAL(previous()), &m_windowStack, SLOT(popWindow()));
	connect(window, SIGNAL(canceled()), this, SLOT(cancelWizzard()));

	window->setContent(func(window));
	window->setPreferredSize(QSize(580, 340));
	window->setup();

	m_windowStack.pushWindow(window);

	return window;
}

QtProjectWizzardWindow* QtProjectWizzard::createWindowWithSummary(
	std::function<void(QtProjectWizzardWindow*, QtProjectWizzardContentSummary*)> func
){
	QtProjectWizzardWindow* window = new QtProjectWizzardWindow(parentWidget());

	connect(window, SIGNAL(previous()), &m_windowStack, SLOT(popWindow()));
	connect(window, SIGNAL(canceled()), this, SLOT(cancelWizzard()));

	QtProjectWizzardContentSummary* summary = new QtProjectWizzardContentSummary(window);

	window->setContent(summary);
	window->setPreferredSize(QSize(750, 500));
	func(window, summary);

	m_windowStack.pushWindow(window);

	return window;
}

void QtProjectWizzard::cancelWizzard()
{
	m_windowStack.clearWindows();
	emit canceled();
}

void QtProjectWizzard::finishWizzard()
{
	m_windowStack.clearWindows();
	emit finished();
}

void QtProjectWizzard::windowStackChanged()
{
	QWidget* window = m_windowStack.getTopWindow();

	if (window)
	{
		dynamic_cast<QtProjectWizzardWindow*>(window)->content()->load();
	}
}

void QtProjectWizzard::selectedProjectType(SourceGroupType sourceGroupType)
{
	m_projectSettings = std::make_shared<ProjectSettings>();

	const std::string sourceGroupId = UUIDUtility::getUUIDString();

	switch (sourceGroupType)
	{
	case SOURCE_GROUP_C_EMPTY:
	case SOURCE_GROUP_CPP_EMPTY:
		m_sourceGroupSettings = std::make_shared<SourceGroupSettingsCxx>(sourceGroupId, sourceGroupType, m_projectSettings.get());
		if (applicationSettingsContainVisualStudioHeaderSearchPaths())
		{
			std::vector<std::string> flags;
			flags.push_back("-fms-extensions");
			flags.push_back("-fms-compatibility");
			flags.push_back("-fms-compatibility-version=19");
			std::dynamic_pointer_cast<SourceGroupSettingsCxx>(m_sourceGroupSettings)->setCompilerFlags(flags);
		}
		emptyProject();
		break;
	case SOURCE_GROUP_CXX_CDB:
		m_sourceGroupSettings = std::make_shared<SourceGroupSettingsCxx>(sourceGroupId, sourceGroupType, m_projectSettings.get());
		emptyProjectCDB();
		break;
	case SOURCE_GROUP_CXX_VS:
		m_sourceGroupSettings = std::make_shared<SourceGroupSettingsCxx>(sourceGroupId, SOURCE_GROUP_CXX_CDB, m_projectSettings.get());
		emptyProjectCDBVS();
		break;
	case SOURCE_GROUP_JAVA_EMPTY:
		m_sourceGroupSettings = std::make_shared<SourceGroupSettingsJava>(sourceGroupId, sourceGroupType, m_projectSettings.get());
		emptyProject();
		break;
	case SOURCE_GROUP_JAVA_MAVEN:
		m_sourceGroupSettings = std::make_shared<SourceGroupSettingsJava>(sourceGroupId, sourceGroupType, m_projectSettings.get());
		emptyProjectJavaMaven();
		break;
	}
}

void QtProjectWizzard::emptyProject()
{
	QtProjectWizzardWindow* window;

	switch (m_sourceGroupSettings->getType())
	{
	case SOURCE_GROUP_C_EMPTY:
	case SOURCE_GROUP_CPP_EMPTY:
		window = createWindowWithContent(
			[this](QtProjectWizzardWindow* window)
			{
				return new QtProjectWizzardContentData(m_projectSettings, m_sourceGroupSettings, window);
			}
		);
		connect(window, SIGNAL(next()), this, SLOT(sourcePaths()));
		break;
	case SOURCE_GROUP_JAVA_EMPTY:
		window = createWindowWithContent(
			[this](QtProjectWizzardWindow* window)
			{
				return new QtProjectWizzardContentData(m_projectSettings, m_sourceGroupSettings, window);
			}
		);
		connect(window, SIGNAL(next()), this, SLOT(sourcePathsJava()));
		break;
	}
	window->updateSubTitle("Project Data");
}

void QtProjectWizzard::emptyProjectCDBVS()
{
	QtProjectWizzardWindow* window = window = createWindowWithContent(
		[this](QtProjectWizzardWindow* window)
		{
			return new QtProjectWizzardContentDataCDBVS(m_projectSettings, m_sourceGroupSettings, window);
		}
	);
	connect(window, SIGNAL(next()), this, SLOT(headerPathsCDB()));
	window->updateSubTitle("Project Data");
}

void QtProjectWizzard::emptyProjectCDB()
{
	QtProjectWizzardWindow* window = createWindowWithContent(
		[this](QtProjectWizzardWindow* window)
		{
			return new QtProjectWizzardContentDataCDB(m_projectSettings, m_sourceGroupSettings, window);
		}
	);
	connect(window, SIGNAL(next()), this, SLOT(headerPathsCDB()));
	window->updateSubTitle("Project Data");
}

void QtProjectWizzard::emptyProjectJavaMaven()
{
	QtProjectWizzardWindow* window = window = createWindowWithContent(
		[this](QtProjectWizzardWindow* window)
		{
			return new QtProjectWizzardContentData(m_projectSettings, m_sourceGroupSettings, window);
		}
	);
	connect(window, SIGNAL(next()), this, SLOT(sourcePathsJavaMaven()));
	window->updateSubTitle("Project Data");
}

void QtProjectWizzard::sourcePaths()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentPathsSource(m_sourceGroupSettings, window));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentExtensions(m_sourceGroupSettings, window));

			window->setup();
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(headerSearchPaths()));
	window->updateSubTitle("Indexed Paths");
}

void QtProjectWizzard::headerSearchPaths()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentPathsHeaderSearch(m_sourceGroupSettings, window));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathsHeaderSearchGlobal(window));

			window->setup();
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(headerSearchPathsDone()));
	window->updateSubTitle("Include Paths");
}

void QtProjectWizzard::headerSearchPathsDone()
{
	if (QSysInfo::macVersion() != QSysInfo::MV_None)
	{
		frameworkSearchPaths();
	}
	else
	{
		advancedSettingsCxx();
	}
}

void QtProjectWizzard::frameworkSearchPaths()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentPathsFrameworkSearch(m_sourceGroupSettings, window));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathsFrameworkSearchGlobal(window));

			window->setup();
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(advancedSettingsCxx()));
	window->updateSubTitle("Framework Search Paths");
}

void QtProjectWizzard::headerPathsCDB()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentCDBSource(m_sourceGroupSettings, window));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathsCDBHeader(m_sourceGroupSettings, window));

			window->setup();
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(advancedSettingsCxx()));
	window->updateSubTitle("Indexed Header Paths");
}

void QtProjectWizzard::sourcePathsJava()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentPathsSource(m_sourceGroupSettings, window));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathsClassJava(m_sourceGroupSettings, window));

			window->setup();
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(advancedSettingsJava()));
	window->updateSubTitle("Indexed Paths");
}

void QtProjectWizzard::sourcePathsJavaMaven()
{
	std::dynamic_pointer_cast<SourceGroupSettingsJava>(m_sourceGroupSettings)->setMavenDependenciesDirectory(
		"./coati_dependencies/" + utility::replace(m_projectSettings->getProjectName(), " ", "_") + "/maven"
	);

	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentPathSourceMaven(m_sourceGroupSettings, window));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathDependenciesMaven(m_sourceGroupSettings, window));

			window->setup();
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(advancedSettingsJava()));
	window->updateSubTitle("Indexed Paths");
}

void QtProjectWizzard::advancedSettingsCxx()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentFlags(m_sourceGroupSettings, window));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathsExclude(m_sourceGroupSettings, window));

			window->setup();
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(showSummary()));
	window->updateSubTitle("Advanced (optional)");
}

void QtProjectWizzard::advancedSettingsJava()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentExtensions(m_sourceGroupSettings, window));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathsExclude(m_sourceGroupSettings, window));

			window->setup();
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(showSummaryJava()));
	window->updateSubTitle("Advanced (optional)");
}

void QtProjectWizzard::showSummary()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->setIsForm(true);

			const bool isCDB = m_sourceGroupSettings->getType() == SOURCE_GROUP_CXX_CDB || m_sourceGroupSettings->getType() == SOURCE_GROUP_CXX_CDB;

			if (!isCDB)
			{
				summary->addContent(new QtProjectWizzardContentData(m_projectSettings, m_sourceGroupSettings, window, m_editing));
				summary->addSpace();

				summary->addContent(new QtProjectWizzardContentPathsSource(m_sourceGroupSettings, window));
				summary->addContent(new QtProjectWizzardContentExtensions(m_sourceGroupSettings, window));
				summary->addSpace();
			}
			else
			{
				summary->addContent(new QtProjectWizzardContentDataCDB(m_projectSettings, m_sourceGroupSettings, window, m_editing));
				summary->addSpace();

				summary->addContent(new QtProjectWizzardContentPathsCDBHeader(m_sourceGroupSettings, window));
				summary->addSpace();
			}

			summary->addContent(new QtProjectWizzardContentPathsHeaderSearch(m_sourceGroupSettings, window, isCDB));

			std::shared_ptr<SourceGroupSettingsCxx> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(m_sourceGroupSettings);
			if (!isCDB && cxxSettings && cxxSettings->getHasDefinedUseSourcePathsForHeaderSearch())
			{
				summary->addSpace();
				summary->addContent(new QtProjectWizzardContentSimple(m_sourceGroupSettings, window));
			}

			summary->addContent(new QtProjectWizzardContentPathsHeaderSearchGlobal(window));
			summary->addSpace();

			if (QSysInfo::macVersion() != QSysInfo::MV_None)
			{
				summary->addContent(new QtProjectWizzardContentPathsFrameworkSearch(m_sourceGroupSettings, window, isCDB));
				summary->addContent(new QtProjectWizzardContentPathsFrameworkSearchGlobal(window));
				summary->addSpace();
			}

			summary->addContent(new QtProjectWizzardContentFlags(m_sourceGroupSettings, window));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathsExclude(m_sourceGroupSettings, window));

			window->setup();

			if (m_editing)
			{
				window->updateTitle("EDIT PROJECT");
				window->updateNextButton("Save");
				window->setPreviousVisible(false);
			}
			else
			{
				window->updateSubTitle("Summary");
				window->updateNextButton("Create");
			}
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(createProject()));
}

void QtProjectWizzard::showSummaryJava()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->setIsForm(true);

			bool isMaven = false;
			std::shared_ptr<SourceGroupSettingsJava> javaSettings = std::dynamic_pointer_cast<SourceGroupSettingsJava>(m_sourceGroupSettings);
			if (javaSettings)
			{
				isMaven = javaSettings->getAbsoluteMavenProjectFilePath().exists();
			}


			summary->addContent(new QtProjectWizzardContentData(m_projectSettings, m_sourceGroupSettings, window, m_editing));
			summary->addSpace();

			if (isMaven)
			{
				summary->addContent(new QtProjectWizzardContentPathSourceMaven(m_sourceGroupSettings, window));
				summary->addSpace();
				summary->addContent(new QtProjectWizzardContentPathDependenciesMaven(m_sourceGroupSettings, window));
			}
			else
			{
				summary->addContent(new QtProjectWizzardContentPathsSource(m_sourceGroupSettings, window));
				summary->addSpace();
				summary->addContent(new QtProjectWizzardContentPathsClassJava(m_sourceGroupSettings, window));
			}

			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentExtensions(m_sourceGroupSettings, window));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathsExclude(m_sourceGroupSettings, window));

			window->setup();

			if (m_editing)
			{
				window->updateTitle("EDIT PROJECT");
				window->updateNextButton("Save");
				window->setPreviousVisible(false);
			}
			else
			{
				window->updateSubTitle("Summary");
				window->updateNextButton("Create");
			}
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(createProject()));
}

void QtProjectWizzard::createProject()
{
	FilePath path = m_projectSettings->getFilePath();

	m_projectSettings->setVersion(ProjectSettings::VERSION);
	m_projectSettings->setAllSourceGroupSettings({m_sourceGroupSettings});
	m_projectSettings->save(path);

	bool forceRefreshProject = false;
	if (m_editing)
	{
		bool settingsChanged = false;

		Application* application = Application::getInstance().get();
		if (application->getCurrentProject() != NULL)
		{
			settingsChanged = !(application->getCurrentProject()->settingsEqualExceptNameAndLocation(*(m_projectSettings.get())));
		}

		bool appSettingsChanged = !(m_appSettings == *ApplicationSettings::getInstance().get());

		if (settingsChanged || appSettingsChanged)
		{
			forceRefreshProject = true;
		}
	}
	else
	{
		MessageStatus("Created project: " + path.str()).dispatch();
	}

	MessageLoadProject(path, forceRefreshProject).dispatch();

	finishWizzard();
}

void QtProjectWizzard::savePreferences()
{
	bool appSettingsChanged = !(m_appSettings == *ApplicationSettings::getInstance().get());

	if (m_appSettings.getScrollSpeed() != ApplicationSettings::getInstance()->getScrollSpeed())
	{
		MessageScrollSpeedChange(ApplicationSettings::getInstance()->getScrollSpeed()).dispatch();
	}

	if (m_appSettings.getCoatiPort() != ApplicationSettings::getInstance()->getCoatiPort() ||
		m_appSettings.getPluginPort() != ApplicationSettings::getInstance()->getPluginPort())
	{
		MessagePluginPortChange().dispatch();
	}

	Application::getInstance()->loadSettings();

	if (appSettingsChanged)
	{
		Project* currentProject = Application::getInstance()->getCurrentProject().get();
		if (currentProject)
		{
			MessageLoadProject(currentProject->getProjectSettingsFilePath(), true).dispatch();
		}
	}
	else
	{
		MessageRefresh().refreshUiOnly().dispatch();
	}

	cancelWizzard();
}
