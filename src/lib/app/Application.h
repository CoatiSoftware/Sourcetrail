#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

#include "../component/view/DialogView.h"
#include "../utility/messaging/type/MessageActivateWindow.h"
#include "../utility/messaging/type/MessageCloseProject.h"
#include "../utility/messaging/type/indexing/MessageIndexingFinished.h"
#include "../utility/messaging/MessageListener.h"
#include "../utility/messaging/type/MessageLoadProject.h"
#include "../utility/messaging/type/MessageRefresh.h"
#include "../utility/messaging/type/MessageRefreshUI.h"
#include "../utility/messaging/type/MessageSwitchColorScheme.h"
#include "../utility/messaging/type/MessageWindowFocus.h"
#include "../project/Project.h"

class Bookmark;
class IDECommunicationController;
class MainView;
class NetworkFactory;
class StorageCache;
class UpdateChecker;
class Version;
class ViewFactory;

class Application
	: public MessageListener<MessageActivateWindow>
	, public MessageListener<MessageCloseProject>
	, public MessageListener<MessageIndexingFinished>
	, public MessageListener<MessageLoadProject>
	, public MessageListener<MessageRefresh>
	, public MessageListener<MessageRefreshUI>
	, public MessageListener<MessageSwitchColorScheme>
	, public MessageListener<MessageWindowFocus>
{
public:
	static void createInstance(
		const Version& version, ViewFactory* viewFactory, NetworkFactory* networkFactory);
	static std::shared_ptr<Application> getInstance();
	static void destroyInstance();

	static std::string getUUID();

	static void loadSettings();
	static void loadStyle(const FilePath& colorSchemePath);

	~Application();

	std::shared_ptr<const Project> getCurrentProject() const;
	FilePath getCurrentProjectPath() const;
	bool isProjectLoaded() const;

	bool hasGUI();

	int handleDialog(const std::wstring& message);
	int handleDialog(const std::wstring& message, const std::vector<std::wstring>& options);
	std::shared_ptr<DialogView> getDialogView(DialogView::UseCase useCase);

	void updateHistoryMenu(std::shared_ptr<MessageBase> message);
	void updateBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks);

private:
	static std::shared_ptr<Application> s_instance;
	static std::string s_uuid;

	Application(bool withGUI = true);

	void handleMessage(MessageActivateWindow* message) override;
	void handleMessage(MessageCloseProject* message) override;
	void handleMessage(MessageIndexingFinished* message) override;
	void handleMessage(MessageLoadProject* message) override;
	void handleMessage(MessageRefresh* message) override;
	void handleMessage(MessageRefreshUI* message) override;
	void handleMessage(MessageSwitchColorScheme* message) override;
	void handleMessage(MessageWindowFocus* message) override;

	void startMessagingAndScheduling();

	void loadWindow(bool showStartWindow);

	void refreshProject(RefreshMode refreshMode, bool shallowIndexingRequested);
	void updateRecentProjects(const FilePath& projectSettingsFilePath);

	void logStorageStats() const;

	void updateTitle();

	bool checkSharedMemory();

	const bool m_hasGUI;
	bool m_loadedWindow = false;

	std::shared_ptr<Project> m_project;
	std::shared_ptr<StorageCache> m_storageCache;

	std::shared_ptr<MainView> m_mainView;

	std::shared_ptr<IDECommunicationController> m_ideCommunicationController;
	std::shared_ptr<UpdateChecker> m_updateChecker;
};

#endif	  // APPLICATION_H
