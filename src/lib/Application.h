#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

#include "component/ComponentManager.h"
#include "component/view/ViewManager.h"
#include "data/access/CodeAccess.h"
#include "data/access/GraphAccess.h"
#include "gui/GuiElementFactory.h"
#include "Project.h"

class Application
{
public:
	static std::shared_ptr<Application> create(std::shared_ptr<GuiElementFactory> guiElementFactory, std::shared_ptr<GuiWindow> window);

	~Application();

	void loadProject();

private:
	Application();

	std::shared_ptr<Project> m_project;

	std::shared_ptr<CodeAccess> m_codeAccess;
	std::shared_ptr<GraphAccess> m_graphAccess;

	std::shared_ptr<ComponentManager> m_componentManager;
	std::shared_ptr<ViewManager> m_viewManager;
};


#endif // APPLICATION_H
