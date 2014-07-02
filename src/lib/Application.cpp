#include "Application.h"

#include "component/view/MainView.h"
#include "gui/GuiFactory.h"
#include "utility/logging/ConsoleLogger.h"
#include "utility/logging/LogManager.h"

std::shared_ptr<Application> Application::create(GuiFactory* guiFactory)
{
	std::shared_ptr<ConsoleLogger> consoleLogger = std::make_shared<ConsoleLogger>();
	LogManager::getInstance()->addLogger(consoleLogger);

	std::shared_ptr<Application> ptr(new Application());

	ptr->m_codeAccess = std::make_shared<CodeAccess>();
	ptr->m_graphAccess = std::make_shared<GraphAccess>();

	ptr->m_mainView = guiFactory->createMainView();
	ptr->m_componentManager =
		ComponentManager::create(guiFactory, ptr->m_mainView.get(), ptr->m_codeAccess, ptr->m_graphAccess);

	return ptr;
}

Application::Application()
{
}

Application::~Application()
{
}

void Application::loadProject()
{
	m_componentManager->setup();
	m_project = Project::create(m_codeAccess, m_graphAccess);

	m_project->loadProjectSettings("data/ProjectSettings.xml");
	m_project->parseCode();
}
