#include  "Application.h"

std::shared_ptr<Application> Application::create(std::shared_ptr<GuiElementFactory> guiElementFactory)
{
	std::shared_ptr<Application> ptr(new Application());
	ptr->m_viewManager = std::make_shared<ViewManager>();
	ptr->m_codeAccess = std::make_shared<CodeAccess>();
	ptr->m_graphAccess = std::make_shared<GraphAccess>();
	ptr->m_componentManager =
		ComponentManager::create(ptr->m_viewManager, guiElementFactory, ptr->m_codeAccess, ptr->m_graphAccess);
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
}
