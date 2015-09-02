#include "Application.h"

#include "utility/logging/logging.h"
#include "utility/messaging/MessageQueue.h"
#include "utility/messaging/type/MessageActivateNode.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/scheduling/TaskScheduler.h"

#include "component/view/GraphViewStyle.h"
#include "component/view/MainView.h"
#include "component/view/ViewFactory.h"
#include "data/StorageCache.h"
#include "settings/ApplicationSettings.h"
#include "settings/ColorScheme.h"

std::shared_ptr<Application> Application::create(ViewFactory* viewFactory)
{
	loadSettings();

	std::shared_ptr<Application> ptr(new Application());

	ptr->m_storageCache = std::make_shared<StorageCache>();

	ptr->m_componentManager = ComponentManager::create(viewFactory, ptr->m_storageCache.get());

	ptr->m_mainView = viewFactory->createMainView();
	ptr->m_componentManager->setup(ptr->m_mainView.get());
	ptr->m_mainView->loadLayout();

	return ptr;
}

void Application::loadSettings()
{
	ApplicationSettings::getInstance()->load("data/ApplicationSettings.xml");
	ColorScheme::getInstance()->load(ApplicationSettings::getInstance()->getColorSchemePath());

	GraphViewStyle::loadStyleSettings();
}

Application::Application()
	: m_isInitialParse(true)
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

	m_storageCache->clear();
	m_componentManager->refreshViews();

	m_project = Project::create(m_storageCache.get());
	m_project->loadProjectSettings(projectSettingsFilePath);
	m_project->parseCode();

	updateRecentProjects(projectSettingsFilePath);
}

void Application::loadSource(const std::string& sourceDirectoryPath)
{
	MessageStatus("Loading Source: " + sourceDirectoryPath).dispatch();

	m_storageCache->clear();
	m_componentManager->refreshViews();

	m_project = Project::create(m_storageCache.get());

	m_project->clearProjectSettings();
	m_project->setSourceDirectoryPath(sourceDirectoryPath);
	m_project->parseCode();
}

void Application::reloadProject()
{
	MessageStatus("Refreshing Project").dispatch();

	m_storageCache->clear();
	m_componentManager->refreshViews();

	m_project->reloadProjectSettings();
	m_project->parseCode();
}

void Application::saveProject(const std::string& projectSettingsFilePath)
{
	if (!m_project->saveProjectSettings(projectSettingsFilePath))
	{
		LOG_ERROR("No Project Settings File defined");
	}
}

void Application::handleMessage(MessageFinishedParsing* message)
{
	m_project->logStats();

	if (!m_isInitialParse || message->errorCount > 0)
	{
		return;
	}

	m_isInitialParse = false;

	Id mainId = m_storageCache->getIdForNodeWithName("main");

	if (!mainId)
	{
		mainId = 1;
	}

	if (mainId)
	{
		MessageActivateNode message(
			mainId,
			m_storageCache->getNodeTypeForNodeWithId(mainId),
			m_storageCache->getNameForNodeWithId(mainId)
		);
		message.isFromSystem = true;
		message.dispatch();
	}
}

void Application::handleMessage(MessageLoadProject* message)
{
	m_isInitialParse = true;
	loadProject(message->projectSettingsFilePath);
}

void Application::handleMessage(MessageLoadSource* message)
{
	m_isInitialParse = true;
	loadSource(message->sourceDirectoryPath);
}

void Application::handleMessage(MessageRefresh* message)
{
	loadSettings();

	if (message->uiOnly)
	{
		m_componentManager->refreshViews();
		return;
	}

	reloadProject();
}

void Application::handleMessage(MessageSaveProject* message)
{
	saveProject(message->projectSettingsFilePath);
}

void Application::updateRecentProjects(const std::string& projectSettingsFilePath)
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
	std::vector<std::string> recentProjects = appSettings->getRecentProjects();
	if (recentProjects.size())
	{
		std::vector<std::string>::iterator it = std::find(recentProjects.begin(), recentProjects.end(), projectSettingsFilePath);
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
	appSettings->save("data/ApplicationSettings.xml");
}
