#include "Application.h"

#include "ApplicationSettings.h"
#include "AppPath.h"
#include "ColorScheme.h"
#include "DialogView.h"
#include "FileSystem.h"
#include "GraphViewStyle.h"
#include "IDECommunicationController.h"
#include "LicenseChecker.h"
#include "logging.h"
#include "LogManager.h"
#include "MainView.h"
#include "MessageFilterErrorCountUpdate.h"
#include "MessageFilterFocusInOut.h"
#include "MessageFilterSearchAutocomplete.h"
#include "MessageQueue.h"
#include "MessageQuitApplication.h"
#include "MessageStatus.h"
#include "NetworkFactory.h"
#include "ProjectSettings.h"
#include "SharedMemoryGarbageCollector.h"
#include "StorageCache.h"
#include "TabId.h"
#include "TaskManager.h"
#include "TaskScheduler.h"
#include "tracing.h"
#include "UpdateChecker.h"
#include "UserPaths.h"
#include "utilityString.h"
#include "utilityUuid.h"
#include "Version.h"
#include "ViewFactory.h"

std::shared_ptr<Application> Application::s_instance;
std::string Application::s_uuid;
bool Application::EULA_ACCEPT_REQUIRED = true;

void Application::createInstance(
	const Version& version, ViewFactory* viewFactory, NetworkFactory* networkFactory
){
	bool hasGui = (viewFactory != nullptr);

	Version::setApplicationVersion(version);
	LicenseChecker::loadPublicKey();
	LicenseChecker::setEncodeKey(AppPath::getAppPath().str());

	if (hasGui)
	{
		GraphViewStyle::setImpl(viewFactory->createGraphStyleImpl());
	}

	loadSettings();

	SharedMemoryGarbageCollector* collector = SharedMemoryGarbageCollector::createInstance();
	if (collector)
	{
		collector->run(Application::getUUID());
	}

	TaskManager::createScheduler(TabId::app());
	TaskManager::createScheduler(TabId::background());
	MessageQueue::getInstance();

	s_instance = std::shared_ptr<Application>(new Application(hasGui));

	s_instance->m_storageCache = std::make_shared<StorageCache>();

	if (hasGui)
	{
		s_instance->m_mainView = viewFactory->createMainView(s_instance->m_storageCache.get());
		s_instance->m_mainView->setup();
	}

	if (networkFactory != nullptr)
	{
		s_instance->m_ideCommunicationController =
			networkFactory->createIDECommunicationController(s_instance->m_storageCache.get());
		s_instance->m_ideCommunicationController->startListening();

		s_instance->m_updateChecker = networkFactory->createUpdateChecker();
	}

	s_instance->startMessagingAndScheduling();
}

std::shared_ptr<Application> Application::getInstance()
{
	return s_instance;
}

void Application::destroyInstance()
{
	MessageQueue::getInstance()->stopMessageLoop();
	TaskManager::destroyScheduler(TabId::background());
	TaskManager::destroyScheduler(TabId::app());

	s_instance.reset();
}

std::string Application::getUUID()
{
	if (!s_uuid.size())
	{
		s_uuid = utility::getUuidString();
	}

	return s_uuid;
}

void Application::loadSettings()
{
	MessageStatus(L"Load settings: " + UserPaths::getAppSettingsPath().wstr()).dispatch();

	std::shared_ptr<ApplicationSettings> settings = ApplicationSettings::getInstance();
	settings->load(UserPaths::getAppSettingsPath());

	LogManager::getInstance()->setLoggingEnabled(settings->getLoggingEnabled());

	loadStyle(settings->getColorSchemePath());
}

void Application::loadStyle(const FilePath& colorSchemePath)
{
	ColorScheme::getInstance()->load(colorSchemePath);
	GraphViewStyle::loadStyleSettings();
}

Application::Application(bool withGUI)
	: m_hasGUI(withGUI)
	, m_lastLicenseCheck(TimeStamp::now())
{
}

Application::~Application()
{
	if (m_hasGUI)
	{
		m_mainView->saveLayout();
	}

	SharedMemoryGarbageCollector* collector = SharedMemoryGarbageCollector::getInstance();
	if (collector)
	{
		collector->stop();
	}
}

std::shared_ptr<const Project> Application::getCurrentProject() const
{
	return m_project;
}

FilePath Application::getCurrentProjectPath() const
{
	if (m_project)
	{
		return m_project->getProjectSettingsFilePath();
	}

	return FilePath();
}

bool Application::isProjectLoaded() const
{
	if (m_project)
	{
		return m_project->isLoaded();
	}
	return false;
}

bool Application::hasGUI()
{
	return m_hasGUI;
}

int Application::handleDialog(const std::wstring& message)
{
	return getDialogView(DialogView::UseCase::GENERAL)->confirm(message);
}

int Application::handleDialog(const std::wstring& message, const std::vector<std::wstring>& options)
{
	return getDialogView(DialogView::UseCase::GENERAL)->confirm(message, options);
}

std::shared_ptr<DialogView> Application::getDialogView(DialogView::UseCase useCase)
{
	if (m_mainView)
	{
		return m_mainView->getDialogView(useCase);
	}

	return std::make_shared<DialogView>(useCase, nullptr);
}

void Application::updateHistoryMenu(std::shared_ptr<MessageBase> message)
{
	m_mainView->updateHistoryMenu(message);
}

void Application::updateBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks)
{
	m_mainView->updateBookmarksMenu(bookmarks);
}

void Application::handleMessage(MessageActivateWindow* message)
{
	if (m_hasGUI)
	{
		m_mainView->activateWindow();
	}
}

void Application::handleMessage(MessageCloseProject* message)
{
	if (m_project && m_project->isIndexing())
	{
		MessageStatus(L"Cannot close the project while indexing.", true, false).dispatch();
		return;
	}

	m_project.reset();
	updateTitle();
	m_mainView->clear();
}

void Application::handleMessage(MessageIndexingFinished* message)
{
	logStorageStats();

	if (m_hasGUI)
	{
		MessageRefreshUI().afterIndexing().dispatch();
	}
	else
	{
		MessageQuitApplication().dispatch();
	}
}

void Application::handleMessage(MessageLoadProject* message)
{
	TRACE("app load project");

	FilePath projectSettingsFilePath(message->projectSettingsFilePath);
	loadWindow(projectSettingsFilePath.empty());

	if (projectSettingsFilePath.empty())
	{
		return;
	}

	if (m_project && m_project->isIndexing())
	{
		MessageStatus(L"Cannot load another project while indexing.", true, false).dispatch();
		return;
	}

	if (m_project && projectSettingsFilePath == m_project->getProjectSettingsFilePath())
	{
		if (message->settingsChanged && m_hasGUI)
		{
			m_project->setStateOutdated();
			refreshProject(REFRESH_ALL_FILES);
		}
	}
	else
	{
		MessageStatus(L"Loading Project: " + projectSettingsFilePath.wstr(), false, true).dispatch();

		projectSettingsFilePath = migrateProjectSettings(projectSettingsFilePath);

		m_project.reset();

		if (m_hasGUI)
		{
			m_mainView->clear();
		}

		try
		{
			updateRecentProjects(projectSettingsFilePath);

			m_storageCache->clear();
			m_storageCache->setSubject(std::weak_ptr<StorageAccess>()); // TODO: check if this is really required.

			m_project = std::make_shared<Project>(
				std::make_shared<ProjectSettings>(projectSettingsFilePath), m_storageCache.get(), getUUID(), hasGUI());

			if (m_project)
			{
				m_project->load(getDialogView(DialogView::UseCase::GENERAL));
			}
			else
			{
				LOG_ERROR_STREAM(<< "Failed to load project.");
				MessageStatus(L"Failed to load project: " + projectSettingsFilePath.wstr(), true).dispatch();
			}

			updateTitle();
		}
		catch (std::exception& e)
		{
			LOG_ERROR_STREAM(<< "Failed to load project, exception thrown: " << e.what());
			MessageStatus(L"Failed to load project, exception was thrown: " + projectSettingsFilePath.wstr(), true).dispatch();
		}
		catch (...)
		{
			LOG_ERROR_STREAM(<< "Failed to load project, unknown exception thrown.");
			MessageStatus(L"Failed to load project, unknown exception was thrown: " + projectSettingsFilePath.wstr(), true).dispatch();
		}

		if (message->refreshMode != REFRESH_NONE)
		{
			refreshProject(message->refreshMode);
		}
	}
}

void Application::handleMessage(MessageRefresh* message)
{
	TRACE("app refresh");

	refreshProject(message->all ? REFRESH_ALL_FILES : REFRESH_UPDATED_FILES);
}

void Application::handleMessage(MessageRefreshUI* message)
{
	TRACE("ui refresh");

	if (m_hasGUI)
	{
		updateTitle();

		if (message->loadStyle)
		{
			loadStyle(ApplicationSettings::getInstance()->getColorSchemePath());
		}

		m_mainView->refreshViews();
	}
}

void Application::handleMessage(MessageSwitchColorScheme* message)
{
	MessageStatus(L"Switch color scheme: " + message->colorSchemePath.wstr()).dispatch();

	loadStyle(message->colorSchemePath);
	MessageRefreshUI().noStyleReload().dispatch();
}

void Application::handleMessage(MessageWindowFocus* message)
{
	if (!message->focusIn)
	{
		return;
	}

	if (m_project && ApplicationSettings::getInstance()->getAutomaticUpdateCheck())
	{
		m_updateChecker->checkUpdate();
	}

	if (!ApplicationSettings::getInstance()->getNonCommercialUse() && TimeStamp::now().deltaHours(m_lastLicenseCheck) > 1)
	{
		m_lastLicenseCheck = TimeStamp::now();

		LicenseChecker::LicenseState state = LicenseChecker::checkCurrentLicense();
		if (state != LicenseChecker::LicenseState::VALID)
		{
			m_mainView->forceEnterLicense(LicenseChecker::getLicenseErrorForState(state));
		}
	}
}

FilePath Application::migrateProjectSettings(const FilePath& projectSettingsFilePath) const
{
	if (projectSettingsFilePath.extension() == L".coatiproject")
	{
		MessageStatus(L"Migrating deprecated project file extension \".coatiproject\" to new file extension \".srctrlprj\"").dispatch();
		const FilePath newSettingsPath = projectSettingsFilePath.replaceExtension(ProjectSettings::PROJECT_FILE_EXTENSION);
		{
			FileSystem::rename(projectSettingsFilePath, newSettingsPath);
			const FilePath oldDbPath = projectSettingsFilePath.replaceExtension(L"coatidb");
			if (oldDbPath.exists())
			{
				FileSystem::rename(oldDbPath, oldDbPath.replaceExtension(L"srctrldb"));
			}
		}
		{
			ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
			std::vector<FilePath> recentProjects = appSettings->getRecentProjects();
			std::vector<FilePath>::iterator it = std::find(recentProjects.begin(), recentProjects.end(), projectSettingsFilePath);
			if (it != recentProjects.end())
			{
				recentProjects.erase(it);
			}
			appSettings->setRecentProjects(recentProjects);
			appSettings->save(UserPaths::getAppSettingsPath());
		}
		return newSettingsPath;
	}
	return projectSettingsFilePath;
}

void Application::startMessagingAndScheduling()
{
	TaskManager::getScheduler(TabId::app())->startSchedulerLoopThreaded();
	TaskManager::getScheduler(TabId::background())->startSchedulerLoopThreaded();

	MessageQueue* queue = MessageQueue::getInstance().get();
	queue->addMessageFilter(std::make_shared<MessageFilterErrorCountUpdate>());
	queue->addMessageFilter(std::make_shared<MessageFilterFocusInOut>());
	queue->addMessageFilter(std::make_shared<MessageFilterSearchAutocomplete>());

	queue->setSendMessagesAsTasks(true);
	queue->startMessageLoopThreaded();
}

void Application::loadWindow(bool showStartWindow)
{
	if (!m_hasGUI)
	{
		return;
	}

	if (!m_loadedWindow)
	{
		ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

		// delay first update check by 24 hours at first launch
		if (!appSettings->getLastUpdateCheck().isValid())
		{
			appSettings->setLastUpdateCheck(TimeStamp::now());
			appSettings->save();
		}

		LicenseChecker::LicenseState state = LicenseChecker::LicenseState::VALID;
		std::string licenseError;

		if (!appSettings->getNonCommercialUse())
		{
			state = LicenseChecker::setCurrentLicenseStringEncoded(appSettings->getLicenseString());
			licenseError = LicenseChecker::getLicenseErrorForState(state);

			if (state == LicenseChecker::LicenseState::VALID)
			{
				MessageStatus(L"Found valid license key, unlocked application.").dispatch();
			}
			else if (state == LicenseChecker::LicenseState::EMPTY)
			{
				licenseError = "";
			}
			else if (state == LicenseChecker::LicenseState::MALFORMED)
			{
				licenseError = "Plese re-enter your license key.";
				appSettings->setLicenseString("");
				appSettings->save();
			}
		}

		updateTitle();

		bool showEula = (EULA_ACCEPT_REQUIRED && appSettings->getAcceptedEulaVersion() < EULA_VERSION);
		bool enterLicense = (state != LicenseChecker::LicenseState::VALID);

		m_mainView->loadWindow(showStartWindow, showEula, enterLicense, licenseError);
		m_loadedWindow = true;
	}
	else if (!showStartWindow)
	{
		m_mainView->hideStartScreen();
	}
}

void Application::refreshProject(RefreshMode refreshMode)
{
	if (m_project && checkSharedMemory())
	{
		m_project->refresh(refreshMode, getDialogView(DialogView::UseCase::INDEXING));

		if (!m_hasGUI && !m_project->isIndexing())
		{
			MessageQuitApplication().dispatch();
		}
	}
}

void Application::updateRecentProjects(const FilePath& projectSettingsFilePath)
{
	if (m_hasGUI)
	{
		ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
		std::vector<FilePath> recentProjects = appSettings->getRecentProjects();
		if (recentProjects.size())
		{
			std::vector<FilePath>::iterator it = std::find(recentProjects.begin(), recentProjects.end(), projectSettingsFilePath);
			if (it != recentProjects.end())
			{
				recentProjects.erase(it);
			}
		}

		recentProjects.insert(recentProjects.begin(), projectSettingsFilePath);
		while (recentProjects.size() > appSettings->getMaxRecentProjectsCount())
		{
			recentProjects.pop_back();
		}

		appSettings->setRecentProjects(recentProjects);
		appSettings->save(UserPaths::getAppSettingsPath());

		m_mainView->updateRecentProjectMenu();
	}
}

void Application::logStorageStats() const
{
	if (!ApplicationSettings::getInstance()->getLoggingEnabled())
	{
		return;
	}

	std::stringstream ss;
	StorageStats stats = m_storageCache->getStorageStats();

	ss << "\nGraph:\n";
	ss << "\t" << stats.nodeCount << " Nodes\n";
	ss << "\t" << stats.edgeCount << " Edges\n";

	ss << "\nCode:\n";
	ss << "\t" << stats.fileCount << " Files\n";
	ss << "\t" << stats.fileLOCCount << " Lines of Code\n";


	ErrorCountInfo errorCount = m_storageCache->getErrorCount();

	ss << "\nErrors:\n";
	ss << "\t" << errorCount.total << " Errors\n";
	ss << "\t" << errorCount.fatal << " Fatal Errors\n";

	LOG_INFO(ss.str());
}

void Application::updateTitle()
{
	if (m_hasGUI)
	{
		std::wstring title = L"Sourcetrail";

		switch (LicenseChecker::getCurrentLicenseType())
		{
			case LicenseType::TEST:
				title += L" [test]";
				break;
			case LicenseType::NON_COMMERCIAL:
				title += L" [non-commercial]";
				break;
			case LicenseType::COMMERCIAL:
				break;
		}

		if (m_project)
		{
			FilePath projectPath = m_project->getProjectSettingsFilePath();

			if (!projectPath.empty())
			{
				title += L" - " + projectPath.fileName();
			}
		}

		m_mainView->setTitle(title);
	}
}

bool Application::checkSharedMemory()
{
	std::wstring error = utility::decodeFromUtf8(SharedMemory::checkSharedMemory(getUUID()));
	if (error.size())
	{
		MessageStatus(L"Error on accessing shared memory. Indexing not possible. "
			"Please restart computer or run as admin: " + error, true).dispatch();
		handleDialog(
			L"There was an error accessing shared memory on your computer: " + error + L"\n\n"
			"Project indexing is not possible. Please restart your computer or try running Sourcetrail as admin. If the "
			"issue persists contact mail@sourcetrail.com");
		return false;
	}

	return true;
}
