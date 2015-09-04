#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

#include "component/ComponentManager.h"
#include "Project.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageSaveProject.h"

class ViewFactory;
class MainView;
class StorageCache;

class Application
	: public MessageListener<MessageFinishedParsing>
	, public MessageListener<MessageLoadProject>
	, public MessageListener<MessageRefresh>
	, public MessageListener<MessageSaveProject>
{
public:
	static std::shared_ptr<Application> create(ViewFactory* viewFactory);
	static void loadSettings();

	~Application();

	void loadProject(const std::string& projectSettingsFilePath);
	void loadSource(const std::string& sourceDirectoryPath);
	void saveProject(const std::string& projectSettingsFilePath);
	void reloadProject();

private:
	Application();

	virtual void handleMessage(MessageFinishedParsing* message);
	virtual void handleMessage(MessageLoadProject* message);
	virtual void handleMessage(MessageRefresh* message);
	virtual void handleMessage(MessageSaveProject* message);

	void updateRecentProjects(const std::string& projectSettingsFilePath);

	std::shared_ptr<Project> m_project;
	std::shared_ptr<StorageCache> m_storageCache;

	std::shared_ptr<MainView> m_mainView;
	std::shared_ptr<ComponentManager> m_componentManager;

	bool m_isInitialParse;
};

#endif // APPLICATION_H
