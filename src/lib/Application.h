#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

#include "component/ComponentManager.h"
#include "data/access/CodeAccess.h"
#include "data/access/GraphAccess.h"
#include "Project.h"

class GuiFactory;
class MainView;

class Application
{
public:
	static std::shared_ptr<Application> create(GuiFactory* guiFactory);

	~Application();

	void loadProject();

private:
	Application();

	std::shared_ptr<Project> m_project;

	std::shared_ptr<CodeAccess> m_codeAccess;
	std::shared_ptr<GraphAccess> m_graphAccess;

	std::shared_ptr<MainView> m_mainView;
	std::shared_ptr<ComponentManager> m_componentManager;
};

#endif // APPLICATION_H
