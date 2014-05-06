#include "Application.h"

#include "utility/logging/ConsoleLogger.h"
#include "utility/logging/LogManager.h"

std::shared_ptr<Application> Application::create(std::shared_ptr<GuiElementFactory> guiElementFactory)
{
	std::shared_ptr<Application> ptr(new Application());
	ptr->m_viewManager = std::make_shared<ViewManager>();
	ptr->m_codeAccess = std::make_shared<CodeAccess>();
	ptr->m_graphAccess = std::make_shared<GraphAccess>();
	ptr->m_componentManager =
		ComponentManager::create(ptr->m_viewManager, guiElementFactory, ptr->m_codeAccess, ptr->m_graphAccess);

	std::shared_ptr<ConsoleLogger> consoleLogger = std::make_shared<ConsoleLogger>();
	LogManager::getInstance()->addLogger(consoleLogger);

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
	m_project = Project::create(m_codeAccess, m_graphAccess);

	m_project->parseCode();
}
