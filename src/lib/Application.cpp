#include "Application.h"

#include "ApplicationSettings.h"
#include "component/view/MainView.h"
#include "data/Storage.h"
#include "gui/GuiFactory.h"
#include "utility/logging/ConsoleLogger.h"
#include "utility/logging/LogManager.h"
#include "utility/messaging/MessageQueue.h"

std::shared_ptr<Application> Application::create(GuiFactory* guiFactory)
{
	std::shared_ptr<ConsoleLogger> consoleLogger = std::make_shared<ConsoleLogger>(); // TODO: move to main
	LogManager::getInstance()->addLogger(consoleLogger);

	ApplicationSettings::getInstance()->load("data/ApplicationSettings.xml");

	std::shared_ptr<Application> ptr(new Application());
	ptr->m_storage = std::make_shared<Storage>();

	ptr->m_mainView = guiFactory->createMainView();
	ptr->m_componentManager =
		ComponentManager::create(guiFactory, ptr->m_mainView.get(), ptr->m_storage, ptr->m_storage);

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

void Application::loadProject()
{
	m_componentManager->setup();
	m_project = Project::create(m_storage);

	m_project->loadProjectSettings("data/ProjectSettings.xml");
	m_project->parseCode();
}
