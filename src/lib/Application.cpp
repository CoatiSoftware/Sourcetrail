#include "Application.h"

#include "utility/logging/logging.h"
#include "utility/messaging/MessageQueue.h"
#include "utility/messaging/type/MessageActivateNodes.h"
#include "utility/messaging/type/MessageDispatchWhenLicenseValid.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/messaging/type/MessageShowStartScreen.h"
#include "utility/scheduling/TaskScheduler.h"
#include "utility/UserPaths.h"
#include "utility/Version.h"

#include "component/view/GraphViewStyle.h"
#include "component/controller/NetworkFactory.h"
#include "component/view/MainView.h"
#include "component/view/ViewFactory.h"
#include "data/StorageCache.h"
#include "isTrial.h"
#include "LicenseChecker.h"
#include "settings/ApplicationSettings.h"
#include "settings/ColorScheme.h"
#include "settings/ProjectSettings.h"

std::shared_ptr<Application> Application::create(
	const Version& version, ViewFactory* viewFactory, NetworkFactory* networkFactory
){
	Version::setApplicationVersion(version);
	loadSettings();

	std::shared_ptr<Application> ptr(new Application());

	ptr->m_storageCache = std::make_shared<StorageCache>();

	ptr->m_componentManager = ComponentManager::create(viewFactory, ptr->m_storageCache.get());

	ptr->m_mainView = viewFactory->createMainView();
	ptr->m_mainView->setTitle("Coati");

	MessageDispatchWhenLicenseValid(std::make_shared<MessageShowStartScreen>()).dispatch();

	ptr->m_componentManager->setup(ptr->m_mainView.get());
	ptr->m_mainView->loadLayout();

	ptr->m_project = Project::create(ptr->m_storageCache.get());

	ptr->m_ideCommunicationController = networkFactory->createIDECommunicationController(ptr->m_storageCache.get());

	ptr->startMessagingAndScheduling();

	return ptr;
}

std::shared_ptr<Application> Application::create(const Version& version)
{
	Version::setApplicationVersion(version);
	loadSettings();

	std::shared_ptr<Application> ptr(new Application(false));

	ptr->m_storageCache = std::make_shared<StorageCache>();
	ptr->m_project = Project::create(ptr->m_storageCache.get());

	ptr->startMessagingAndScheduling();

	return ptr;
}

void Application::loadSettings()
{
	ApplicationSettings::getInstance()->load(FilePath(UserPaths::getAppSettingsPath()));

	ColorScheme::getInstance()->load(ApplicationSettings::getInstance()->getColorSchemePath());
	GraphViewStyle::loadStyleSettings();
}

Application::Application(bool withGUI)
	: m_hasGUI(withGUI)
{
	LicenseChecker::createInstance();
}

Application::~Application()
{
	MessageQueue::getInstance()->stopMessageLoop();
	TaskScheduler::getInstance()->stopSchedulerLoop();
	if (m_hasGUI)
	{
		m_mainView->saveLayout();
	}
}

bool Application::hasGUI()
{
	return m_hasGUI;
}

void Application::createAndLoadProject(const FilePath& projectSettingsFilePath)
{
	MessageStatus("Loading Project: " + projectSettingsFilePath.str(), false, true).dispatch();

	loadSettings();
	updateRecentProjects(projectSettingsFilePath);

	m_storageCache->clear();

	m_project = Project::create(m_storageCache.get());
	loadProject(projectSettingsFilePath);

	if (m_hasGUI)
	{
		m_mainView->setTitle("Coati - " + projectSettingsFilePath.fileName());
		m_mainView->hideStartScreen();

		m_componentManager->refreshViews();
	}
}

void Application::loadProject(const FilePath& projectSettingsFilePath)
{
	if (m_hasGUI)
	{
		m_componentManager->clearComponents();
	}

	bool reparse = false;

	Project::ProjectState state = m_project->load(projectSettingsFilePath);
	if (state == Project::PROJECT_OUTDATED)
	{
		if (m_hasGUI && !isTrial())
		{
			std::vector<std::string> options;
			options.push_back("Yes");
			options.push_back("No");
			int result = m_mainView->confirm(
				"The project file was changed after the last indexing. The project needs to get fully reindexed to "
				"reflect the current project state. Do you want to reindex the project?", options);

			reparse = (result == 0);
		}
	}
	else if (state == Project::PROJECT_OUTVERSIONED)
	{
		MessageStatus("Can't load project").dispatch();

		reparse = true;

		if (m_hasGUI)
		{
			std::vector<std::string> options;
			options.push_back("Yes");
			options.push_back("No");
			int result = m_mainView->confirm(
				"This project was indexed with a different version of Coati. It needs to be fully reindexed to be used "
				"with this version of Coati. Do you want to reindex the project?", options);

			reparse = (result == 0);
		}
	}

	if (reparse)
	{
		m_project->clearStorage();
		m_project->load(projectSettingsFilePath);
	}
}

void Application::refreshProject()
{
	MessageStatus("Refreshing Project").dispatch();

	FilePath cdbPath = ProjectSettings::getInstance()->getCompilationDatabasePath();
	if (!cdbPath.empty() && !cdbPath.exists())
	{
		MessageStatus("Can't refresh project").dispatch();

		if (m_hasGUI)
		{
			std::vector<std::string> options;
			options.push_back("Ok");
			m_mainView->confirm(
				"Can't refresh. The compilation database of the project does not exist anymore: " + cdbPath.str(),
				options);
		}

		return;
	}

	m_storageCache->clear();
	if (m_hasGUI)
	{
		m_componentManager->refreshViews();
	}

	Project::ProjectState state = m_project->reload();
	if (state != Project::PROJECT_LOADED)
	{
		MessageStatus("Can't refresh project").dispatch();
		loadProject(m_project->getProjectSettingsFilePath());
	}
}

void Application::saveProject(const FilePath& projectSettingsFilePath)
{
	if (!m_project->save(projectSettingsFilePath))
	{
		LOG_ERROR("No Project Settings File defined");
	}
	else
	{
		MessageStatus("Project saved").dispatch();
	}
}

void Application::handleMessage(MessageActivateWindow* message)
{
	if (m_hasGUI)
	{
		m_mainView->activateWindow();
	}
}

void Application::handleMessage(MessageFinishedParsing* message)
{
	m_project->logStats();
	if (m_hasGUI)
	{
		MessageRefresh().refreshUiOnly().dispatch();
	}
}

void Application::handleMessage(MessageLoadProject* message)
{
	FilePath projectSettingsFilePath(message->projectSettingsFilePath);
	if (projectSettingsFilePath.empty())
	{
		projectSettingsFilePath = m_project->getProjectSettingsFilePath();
		if (projectSettingsFilePath.empty())
		{
			return;
		}
	}

	if (message->forceRefresh && !isTrial())
	{
		if (m_hasGUI)
		{
			std::vector<std::string> options;
			options.push_back("Yes");
			options.push_back("No");
			int result = m_mainView->confirm(
				"Some settings were changed, the project needs to be fully reindexed. "
				"Do you want to reindex the project?", options);

			if (result == 1)
			{
				m_project->load(projectSettingsFilePath);
				return;
			}
		}

		m_project->clearStorage();
	}
	else if (projectSettingsFilePath == m_project->getProjectSettingsFilePath())
	{
		return;
	}

	try
	{
		createAndLoadProject(projectSettingsFilePath);
	}
	catch (...)
	{
		LOG_ERROR_STREAM(<< "Failed to load project. Try restarting Coati as admin");
		MessageStatus("Failed to load project. Try restarting Coati as admin", true).dispatch();
	}
}

void Application::handleMessage(MessageRefresh* message)
{
	loadSettings();

	if (message->uiOnly)
	{
		m_componentManager->refreshViews();
	}
	else if (message->all)
	{
		m_project->clearStorage();
		refreshProject();
	}
	else
	{
		refreshProject();
	}
}

void Application::handleMessage(MessageSaveProject* message)
{
	saveProject(message->projectSettingsFilePath);
}

void Application::startMessagingAndScheduling()
{
	TaskScheduler::getInstance()->startSchedulerLoopThreaded();
	MessageQueue::getInstance()->setSendMessagesAsTasks(true);
	MessageQueue::getInstance()->startMessageLoopThreaded();
}

void Application::updateRecentProjects(const FilePath& projectSettingsFilePath)
{
	if (m_hasGUI)
	{
		ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
		std::vector<FilePath> recentProjects = appSettings->getRecentProjects();
		if (recentProjects.size())
		{
			std::vector<FilePath>::iterator it = std::find(recentProjects.begin(), recentProjects.end(), projectSettingsFilePath);
			if (it != recentProjects.end())
			{
				recentProjects.erase(it);
			}
		}

		recentProjects.insert(recentProjects.begin(), projectSettingsFilePath);
		while (recentProjects.size() > 7)
		{
			recentProjects.pop_back();
		}

		appSettings->setRecentProjects(recentProjects);
		appSettings->save(UserPaths::getAppSettingsPath());

		m_mainView->updateRecentProjectMenu();
	}
}
