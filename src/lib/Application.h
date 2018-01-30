#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

#include "component/ComponentManager.h"
#include "project/Project.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateWindow.h"
#include "utility/messaging/type/MessageEnteredLicense.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageSwitchColorScheme.h"
#include "utility/messaging/type/MessageWindowFocus.h"

class Bookmark;
class DialogView;
class IDECommunicationController;
class MainView;
class NetworkFactory;
class StorageCache;
class UpdateChecker;
class Version;
class ViewFactory;

class Application
	: public MessageListener<MessageActivateWindow>
	, public MessageListener<MessageEnteredLicense>
	, public MessageListener<MessageFinishedParsing>
	, public MessageListener<MessageLoadProject>
	, public MessageListener<MessageRefresh>
	, public MessageListener<MessageSwitchColorScheme>
	, public MessageListener<MessageWindowFocus>
{
public:
	static void createInstance(const Version& version, ViewFactory* viewFactory, NetworkFactory* networkFactory);
	static std::shared_ptr<Application> getInstance();
	static void destroyInstance();

	static std::string getUUID();

	static void loadSettings();
	static void loadStyle(const FilePath& colorSchemePath);

	~Application();

	const std::shared_ptr<Project> getCurrentProject();

	void createAndLoadProject(const FilePath& projectSettingsFilePath);
	void refreshProject(RefreshMode refreshMode);
	bool hasGUI();

	int handleDialog(const std::string& message);
	int handleDialog(const std::string& message, const std::vector<std::string>& options);
	int handleDialog(const std::wstring& message);
	int handleDialog(const std::wstring& message, const std::vector<std::wstring>& options);
	std::shared_ptr<DialogView> getDialogView();

	void updateHistory(const std::vector<SearchMatch>& history);
	void updateBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks);

private:
	static std::shared_ptr<Application> s_instance;
	static std::string s_uuid;

	Application(bool withGUI=true);

	virtual void handleMessage(MessageActivateWindow* message);
	virtual void handleMessage(MessageEnteredLicense* message);
	virtual void handleMessage(MessageFinishedParsing* message);
	virtual void handleMessage(MessageLoadProject* message);
	virtual void handleMessage(MessageRefresh* message);
	virtual void handleMessage(MessageSwitchColorScheme* message);
	virtual void handleMessage(MessageWindowFocus* message);

	void startMessagingAndScheduling();

	void updateRecentProjects(const FilePath& projectSettingsFilePath);

	void logStorageStats() const;

	void updateTitle();

	bool checkSharedMemory();

	const bool m_hasGUI;
	std::shared_ptr<Project> m_project;
	std::shared_ptr<StorageCache> m_storageCache;

	std::shared_ptr<MainView> m_mainView;
	std::shared_ptr<ComponentManager> m_componentManager;

	std::shared_ptr<IDECommunicationController> m_ideCommunicationController;
	std::shared_ptr<UpdateChecker> m_updateChecker;

	MessageEnteredLicense::LicenseType m_licenseType;
	TimeStamp m_lastLicenseCheck;
};

#endif // APPLICATION_H
