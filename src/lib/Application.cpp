#include "Application.h"

#include "utility/logging/logging.h"
#include "utility/messaging/MessageQueue.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/scheduling/TaskScheduler.h"

#include "component/view/MainView.h"
#include "component/view/ViewFactory.h"
#include "data/access/StorageAccessProxy.h"
#include "settings/ApplicationSettings.h"

std::shared_ptr<Application> Application::create(ViewFactory* viewFactory)
{
	std::shared_ptr<ApplicationSettings> settings = ApplicationSettings::getInstance();
	settings->load("data/ApplicationSettings.xml");

	std::shared_ptr<Application> ptr(new Application());

	ptr->m_storageAccessProxy = std::make_shared<StorageAccessProxy>();

	ptr->m_componentManager = ComponentManager::create(viewFactory, ptr->m_storageAccessProxy.get());

	ptr->m_mainView = viewFactory->createMainView();
	ptr->m_componentManager->setup(ptr->m_mainView.get());
	ptr->m_mainView->loadLayout();

	std::string startupProjectFilePath = settings->getStartupProjectFilePath();
	if (startupProjectFilePath.size())
	{
		MessageLoadProject(startupProjectFilePath).dispatch();
	}
	else
	{
		MessageStatus("StartupProject was not defined in the ApplicationSettings", true).dispatch();
		LOG_WARNING("No StartupProject defined in ApplicationSettings");
	}

	return ptr;
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

void Application::loadProject(const std::string& projectSettingsFilePath)
{
	MessageStatus("Loading Project: " + projectSettingsFilePath).dispatch();

	m_project = Project::create(m_storageAccessProxy.get());

	m_project->loadProjectSettings(projectSettingsFilePath);
	m_project->parseCode();
}

void Application::loadSource(const std::string& sourceDirectoryPath)
{
	MessageStatus("Loading Source: " + sourceDirectoryPath).dispatch();

	m_project = Project::create(m_storageAccessProxy.get());

	m_project->clearProjectSettings();
	m_project->setSourceDirectoryPath(sourceDirectoryPath);
	m_project->parseCode();
}

void Application::reloadProject()
{
	MessageStatus("Refreshing Project").dispatch();

	m_project->parseCode();
}

void Application::saveProject(const std::string& projectSettingsFilePath)
{
	if(!m_project->saveProjectSettings(projectSettingsFilePath))
	{
		LOG_ERROR("No Project Settings File defined");
	}
}

void Application::handleMessage(MessageFinishedParsing* message)
{
	if (message->errorCount > 0)
	{
		return;
	}

	Id mainId = m_storageAccessProxy->getIdForNodeWithName("main");

	if (!mainId)
	{
		mainId = 1;
	}

	MessageActivateTokens(mainId).dispatch();
}

void Application::handleMessage(MessageLoadProject* message)
{
	loadProject(message->projectSettingsFilePath);
}

void Application::handleMessage(MessageLoadSource* message)
{
	loadSource(message->sourceDirectoryPath);
}

void Application::handleMessage(MessageRefresh* message)
{
	reloadProject();
}

void Application::handleMessage(MessageSaveProject* message)
{
	saveProject(message->projectSettingsFilePath);
}
