#include "Application.h"

#include "ApplicationSettings.h"
#include "component/view/MainView.h"
#include "data/access/GraphAccessProxy.h"
#include "data/access/LocationAccessProxy.h"
#include "gui/GuiFactory.h"
#include "utility/logging/ConsoleLogger.h"
#include "utility/logging/LogManager.h"
#include "utility/messaging/MessageQueue.h"
#include "utility/messaging/type/MessageActivateToken.h"

std::shared_ptr<Application> Application::create(GuiFactory* guiFactory)
{
	std::shared_ptr<ConsoleLogger> consoleLogger = std::make_shared<ConsoleLogger>(); // TODO: move to main
	LogManager::getInstance()->addLogger(consoleLogger);

	ApplicationSettings::getInstance()->load("data/ApplicationSettings.xml");

	std::shared_ptr<Application> ptr(new Application());

	ptr->m_graphAccessProxy = std::make_shared<GraphAccessProxy>();
	ptr->m_locationAccessProxy = std::make_shared<LocationAccessProxy>();

	ptr->m_mainView = guiFactory->createMainView();
	ptr->m_componentManager = ComponentManager::create(
		guiFactory, ptr->m_mainView.get(), ptr->m_graphAccessProxy.get(), ptr->m_locationAccessProxy.get());

	ptr->m_componentManager->setup();

	return ptr;
}

Application::Application()
{
	MessageQueue::getInstance()->startMessageLoopThreaded();
}

Application::~Application()
{
	MessageQueue::getInstance()->stopMessageLoop();
}

void Application::loadProject(const std::string& projectSettingsFilePath)
{
	m_project = Project::create(m_graphAccessProxy.get(), m_locationAccessProxy.get());

	m_project->loadProjectSettings(projectSettingsFilePath);
	m_project->parseCode();

	MessageActivateToken message(1);
	message.dispatch();
}

void Application::loadSource(const std::string& sourceDirectoryPath)
{
	m_project = Project::create(m_graphAccessProxy.get(), m_locationAccessProxy.get());

	m_project->clearProjectSettings();
	m_project->setSourceDirectoryPath(sourceDirectoryPath);
	m_project->parseCode();

	MessageActivateToken message(1);
	message.dispatch();
}

void Application::handleMessage(MessageLoadProject* message)
{
	loadProject(message->projectSettingsFilePath);
}

void Application::handleMessage(MessageLoadSource* message)
{
	loadSource(message->sourceDirectoryPath);
}
