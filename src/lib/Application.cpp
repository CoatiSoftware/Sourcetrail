#include "Application.h"

#include "utility/logging/logging.h"
#include "utility/messaging/MessageQueue.h"
#include "utility/messaging/type/MessageActivateNodes.h"
#include "utility/messaging/type/MessageShowErrors.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/scheduling/TaskScheduler.h"
#include "utility/Version.h"

#include "component/view/GraphViewStyle.h"
#include "component/controller/NetworkFactory.h"
#include "component/view/MainView.h"
#include "component/view/ViewFactory.h"
#include "data/StorageCache.h"
#include "settings/ApplicationSettings.h"
#include "settings/ColorScheme.h"

std::string Application::m_appSettingsPath = "data/ApplicationSettings.xml";

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

	ptr->m_componentManager->setup(ptr->m_mainView.get());
	ptr->m_mainView->loadLayout();

	ptr->m_project = Project::create(ptr->m_storageCache.get());

	std::string startupProjectFilePath = ApplicationSettings::getInstance()->getStartupProjectFilePath();
	if (startupProjectFilePath.size())
	{
		MessageLoadProject(startupProjectFilePath).dispatch();
		ptr->m_mainView->hideStartScreen();
	}

	ptr->m_ideCommunicationController = networkFactory->createIDECommunicationController(ptr->m_storageCache.get());

	return ptr;
}

void Application::loadSettings()
{
	ApplicationSettings::getInstance()->load(FilePath(m_appSettingsPath));
	ColorScheme::getInstance()->load(ApplicationSettings::getInstance()->getColorSchemePath());

	GraphViewStyle::loadStyleSettings();
}

Application::Application()
{
	TaskScheduler::getInstance()->startSchedulerLoopThreaded();
	MessageQueue::getInstance()->setSendMessagesAsTasks(true);
	MessageQueue::getInstance()->startMessageLoopThreaded();
}

Application::~Application()
{
	MessageQueue::getInstance()->stopMessageLoop();
	TaskScheduler::getInstance()->stopSchedulerLoop();
	m_mainView->saveLayout();
}

void Application::loadProject(const FilePath& projectSettingsFilePath)
{
	MessageStatus("Loading Project: " + projectSettingsFilePath.str()).dispatch();

	loadSettings();
	updateRecentProjects(projectSettingsFilePath);

	m_mainView->setTitle(
			projectSettingsFilePath.withoutExtension().fileName()
			+ "[" + projectSettingsFilePath.fileName() + "]" + " - Coati");

	m_storageCache->clear();
	m_componentManager->refreshViews();

	m_project = Project::create(m_storageCache.get());
	m_project->loadProject(projectSettingsFilePath);

	m_mainView->updateRecentProjectMenu();
	m_mainView->hideStartScreen();
}

void Application::refreshProject()
{
	MessageStatus("Refreshing Project").dispatch();

	m_storageCache->clear();
	m_componentManager->refreshViews();

	m_project->reloadProject();
}

void Application::saveProject(const FilePath& projectSettingsFilePath)
{
	if (!m_project->saveProject(projectSettingsFilePath))
	{
		LOG_ERROR("No Project Settings File defined");
	}
}

void Application::setAppSettingsPath(const std::string& appSettingsPath)
{
	m_appSettingsPath = appSettingsPath;
}

void Application::handleMessage(MessageActivateWindow* message)
{
	m_mainView->activateWindow();
}

void Application::handleMessage(MessageFinishedParsing* message)
{
	m_project->logStats();

	if (message->errorCount == 0)
	{
		MessageRefresh().refreshUiOnly().dispatch();
	}
	else
	{
		MessageShowErrors().dispatch();
	}
}

void Application::handleMessage(MessageLoadProject* message)
{
	loadProject(message->projectSettingsFilePath);
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

void Application::updateRecentProjects(const FilePath& projectSettingsFilePath)
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
	if (recentProjects.size() > 7)
	{
		recentProjects.pop_back();
	}

	appSettings->setRecentProjects(recentProjects);
	appSettings->save(m_appSettingsPath);
}
