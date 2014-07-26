#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

#include "component/ComponentManager.h"
#include "Project.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageLoadSource.h"
#include "utility/messaging/type/MessageRefresh.h"

class ViewFactory;
class MainView;
class GraphAccessProxy;
class LocationAccessProxy;

class Application
	: public MessageListener<MessageLoadProject>
	, public MessageListener<MessageLoadSource>
	, public MessageListener<MessageRefresh>
{
public:
	static std::shared_ptr<Application> create(ViewFactory* viewFactory);

	~Application();

	void loadProject(const std::string& projectSettingsFilePath);
	void loadSource(const std::string& sourceDirectoryPath);
	void reloadProject();

private:
	Application();

	virtual void handleMessage(MessageLoadProject* message);
	virtual void handleMessage(MessageLoadSource* message);
	virtual void handleMessage(MessageRefresh* message);

	std::shared_ptr<Project> m_project;
	std::shared_ptr<GraphAccessProxy> m_graphAccessProxy;
	std::shared_ptr<LocationAccessProxy> m_locationAccessProxy;

	std::shared_ptr<MainView> m_mainView;
	std::shared_ptr<ComponentManager> m_componentManager;
};

#endif // APPLICATION_H
