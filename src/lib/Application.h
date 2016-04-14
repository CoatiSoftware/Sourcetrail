#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

#include "component/ComponentManager.h"
#include "Project.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateWindow.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageSaveProject.h"

class IDECommunicationController;
class NetworkFactory;
class ViewFactory;
class MainView;
class StorageCache;
class Version;

class Application
	: public MessageListener<MessageActivateWindow>
	, public MessageListener<MessageFinishedParsing>
	, public MessageListener<MessageLoadProject>
	, public MessageListener<MessageRefresh>
	, public MessageListener<MessageSaveProject>
{
public:
	static std::shared_ptr<Application> create(const Version& version, ViewFactory* viewFactory, NetworkFactory* networkFactory);
	static std::shared_ptr<Application> create(const Version& version);
	static void loadSettings();

	~Application();

	void loadProject(const FilePath& projectSettingsFilePath);
	void refreshProject();
	void saveProject(const FilePath& projectSettingsFilePath);
	void forceEnterLicense();
	bool hasGUI();

private:
	Application(bool withGUI=true);

	virtual void handleMessage(MessageActivateWindow* message);
	virtual void handleMessage(MessageFinishedParsing* message);
	virtual void handleMessage(MessageLoadProject* message);
	virtual void handleMessage(MessageRefresh* message);
	virtual void handleMessage(MessageSaveProject* message);

	void startMessagingAndScheduling();

	void updateRecentProjects(const FilePath& projectSettingsFilePath);

	bool m_hasGUI;
	std::shared_ptr<Project> m_project;
	std::shared_ptr<StorageCache> m_storageCache;

	std::shared_ptr<MainView> m_mainView;
	std::shared_ptr<ComponentManager> m_componentManager;

	std::shared_ptr<IDECommunicationController> m_ideCommunicationController;
};

#endif // APPLICATION_H
