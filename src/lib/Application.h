#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

#include "DialogView.h"
#include "MessageListener.h"
#include "MessageIndexingFinished.h"
#include "MessageActivateWindow.h"
#include "MessageEnteredLicense.h"
#include "MessageLoadProject.h"
#include "MessageRefresh.h"
#include "MessageRefreshUI.h"
#include "MessageSwitchColorScheme.h"
#include "MessageWindowFocus.h"
#include "Project.h"
#include "TimeStamp.h"

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
	, public MessageListener<MessageEnteredLicense>
	, public MessageListener<MessageIndexingFinished>
	, public MessageListener<MessageLoadProject>
	, public MessageListener<MessageRefresh>
	, public MessageListener<MessageRefreshUI>
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
	bool isProjectLoaded() const;

	bool hasGUI();

	int handleDialog(const std::string& message);
	int handleDialog(const std::string& message, const std::vector<std::string>& options);
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
	void handleMessage(MessageEnteredLicense* message) override;
	void handleMessage(MessageIndexingFinished* message) override;
	void handleMessage(MessageLoadProject* message) override;
	void handleMessage(MessageRefresh* message) override;
	void handleMessage(MessageRefreshUI* message) override;
	void handleMessage(MessageSwitchColorScheme* message) override;
	void handleMessage(MessageWindowFocus* message) override;

	FilePath migrateProjectSettings(const FilePath& projectSettingsFilePath) const;
	void startMessagingAndScheduling();

	void refreshProject(RefreshMode refreshMode);
	void updateRecentProjects(const FilePath& projectSettingsFilePath);

	void logStorageStats() const;

	void updateTitle();

	bool checkSharedMemory();

	const bool m_hasGUI;
	std::shared_ptr<Project> m_project;
	std::shared_ptr<StorageCache> m_storageCache;

	std::shared_ptr<MainView> m_mainView;

	std::shared_ptr<IDECommunicationController> m_ideCommunicationController;
	std::shared_ptr<UpdateChecker> m_updateChecker;

	MessageEnteredLicense::LicenseType m_licenseType;
	TimeStamp m_lastLicenseCheck;
};

#endif // APPLICATION_H
