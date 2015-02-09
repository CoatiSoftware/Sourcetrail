#include "Application.h"

#include "ApplicationSettings.h"
#include "component/view/MainView.h"
#include "component/view/ViewFactory.h"
#include "data/access/GraphAccessProxy.h"
#include "data/access/LocationAccessProxy.h"
#include "utility/logging/logging.h"
#include "utility/messaging/MessageQueue.h"
#include "utility/messaging/type/MessageActivateTokens.h"

std::shared_ptr<Application> Application::create(ViewFactory* viewFactory)
{
	ApplicationSettings::getInstance()->load("data/ApplicationSettings.xml");

	std::shared_ptr<Application> ptr(new Application());

	ptr->m_graphAccessProxy = std::make_shared<GraphAccessProxy>();
	ptr->m_locationAccessProxy = std::make_shared<LocationAccessProxy>();

	ptr->m_mainView = viewFactory->createMainView();
	ptr->m_componentManager = ComponentManager::create(
		viewFactory, ptr->m_mainView.get(), ptr->m_graphAccessProxy.get(), ptr->m_locationAccessProxy.get());

	ptr->m_componentManager->setup();
	ptr->m_mainView->loadLayout();

	MessageLoadProject("data/ProjectSettings.xml").dispatch();

	return ptr;
}

Application::Application()
{
	MessageQueue::getInstance()->startMessageLoopThreaded();
}

Application::~Application()
{
	MessageQueue::getInstance()->stopMessageLoop();
	m_mainView->saveLayout();
}

void Application::loadProject(const std::string& projectSettingsFilePath)
{
	m_project = Project::create(m_graphAccessProxy.get(), m_locationAccessProxy.get());

	m_project->loadProjectSettings(projectSettingsFilePath);
	m_project->parseCode();
}

void Application::loadSource(const std::string& sourceDirectoryPath)
{
	m_project = Project::create(m_graphAccessProxy.get(), m_locationAccessProxy.get());

	m_project->clearProjectSettings();
	m_project->setSourceDirectoryPath(sourceDirectoryPath);
	m_project->parseCode();
}

void Application::reloadProject()
{
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

	Id mainId = m_graphAccessProxy->getIdForNodeWithName("main");

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
