#include "Application.h"

#include "utility/interprocess/SharedMemoryGarbageCollector.h"
#include "utility/logging/logging.h"
#include "utility/logging/LogManager.h"
#include "utility/messaging/filter_types/MessageFilterFocusInOut.h"
#include "utility/messaging/filter_types/MessageFilterNewErrors.h"
#include "utility/messaging/filter_types/MessageFilterSearchAutocomplete.h"
#include "utility/messaging/MessageQueue.h"
#include "utility/messaging/type/MessageForceEnterLicense.h"
#include "utility/messaging/type/MessageQuitApplication.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/scheduling/TaskScheduler.h"
#include "utility/tracing.h"
#include "utility/UserPaths.h"
#include "utility/utilityUuid.h"
#include "utility/Version.h"

#include "component/controller/IDECommunicationController.h"
#include "component/NetworkFactory.h"
#include "component/view/DialogView.h"
#include "component/view/GraphViewStyle.h"
#include "component/view/MainView.h"
#include "component/view/ViewFactory.h"
#include "data/storage/StorageCache.h"
#include "LicenseChecker.h"
#include "settings/ApplicationSettings.h"
#include "settings/ProjectSettings.h"
#include "settings/ColorScheme.h"
#include "UpdateChecker.h"

std::shared_ptr<Application> Application::s_instance;
std::string Application::s_uuid;

void Application::createInstance(
	const Version& version, ViewFactory* viewFactory, NetworkFactory* networkFactory
){
	Version::setApplicationVersion(version);
	loadSettings();

	SharedMemoryGarbageCollector* collector = SharedMemoryGarbageCollector::createInstance();
	if (collector)
	{
		collector->run(Application::getUUID());
	}

	TaskScheduler::getInstance();
	MessageQueue::getInstance();

	bool hasGui = (viewFactory != nullptr);
	s_instance = std::shared_ptr<Application>(new Application(hasGui));

	s_instance->m_storageCache = std::make_shared<StorageCache>();

	if (hasGui)
	{
		s_instance->m_componentManager = ComponentManager::create(viewFactory, s_instance->m_storageCache.get());

		s_instance->m_mainView = viewFactory->createMainView();
		s_instance->updateTitle();

		s_instance->m_componentManager->setup(s_instance->m_mainView.get());
		s_instance->m_mainView->loadLayout();

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
	MessageStatus("Load settings: " + UserPaths::getAppSettingsPath().str()).dispatch();

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
	, m_licenseType(MessageEnteredLicense::LICENSE_NONE)
	, m_lastLicenseCheck(TimeStamp::now())
{
	LicenseChecker::createInstance();
}

Application::~Application()
{
	MessageQueue::getInstance()->stopMessageLoop();
	TaskScheduler::getInstance()->stopSchedulerLoop();

	if (m_hasGUI)
	{
		m_mainView->saveLayout();
	}

	SharedMemoryGarbageCollector* collector = SharedMemoryGarbageCollector::createInstance();
	if (collector)
	{
		collector->stop();
	}
}

const std::shared_ptr<Project> Application::getCurrentProject()
{
	return m_project;
}

bool Application::hasGUI()
{
	return m_hasGUI;
}

int Application::handleDialog(const std::string& message)
{
	return getDialogView()->confirm(message);
}

int Application::handleDialog(const std::string& message, const std::vector<std::string>& options)
{
	return getDialogView()->confirm(message, options);
}

std::shared_ptr<DialogView> Application::getDialogView()
{
	if (m_componentManager)
	{
		return m_componentManager->getDialogView();
	}

	return std::make_shared<DialogView>(nullptr);
}

void Application::updateHistory(const std::vector<SearchMatch>& history)
{
	m_mainView->updateHistoryMenu(history);
}

void Application::updateBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks)
{
	m_mainView->updateBookmarksMenu(bookmarks);
}

void Application::createAndLoadProject(const FilePath& projectSettingsFilePath)
{
	MessageStatus("Loading Project: " + projectSettingsFilePath.str(), false, true).dispatch();
	try
	{
		updateRecentProjects(projectSettingsFilePath);

		m_storageCache->clear();
		m_storageCache->setSubject(nullptr);

		m_project = std::make_shared<Project>(
			std::make_shared<ProjectSettings>(projectSettingsFilePath), m_storageCache.get(), hasGUI());

		if (m_project)
		{
			m_project->load();
		}
		else
		{
			LOG_ERROR_STREAM(<< "Failed to load project.");
			MessageStatus("Failed to load project: " + projectSettingsFilePath.str(), true).dispatch();
		}

		updateTitle();
	}
	catch (std::exception& e)
	{
		LOG_ERROR_STREAM(<< "Failed to load project, exception thrown: " << e.what());
		MessageStatus("Failed to load project, exception was thrown: " + projectSettingsFilePath.str(), true).dispatch();
	}
	catch (...)
	{
		LOG_ERROR_STREAM(<< "Failed to load project, unknown exception thrown.");
		MessageStatus("Failed to load project, unknown exception was thrown: " + projectSettingsFilePath.str(), true).dispatch();
	}

	if (m_hasGUI)
	{
		m_componentManager->clearComponents();
	}
}

void Application::refreshProject(RefreshMode refreshMode)
{
	if (m_project && checkSharedMemory())
	{
		m_project->refresh(refreshMode, getDialogView().get());
	}
}

void Application::handleMessage(MessageActivateWindow* message)
{
	if (m_hasGUI)
	{
		m_mainView->activateWindow();
	}
}

void Application::handleMessage(MessageEnteredLicense* message)
{
	MessageStatus("Found valid license key, unlocked application.").dispatch();

	m_licenseType = message->type;

	updateTitle();
}

void Application::handleMessage(MessageFinishedParsing* message)
{
	logStorageStats();

	if (m_hasGUI)
	{
		MessageRefresh().refreshUiOnly().dispatch();
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
	if (projectSettingsFilePath.empty())
	{
		return;
	}

	if (m_project && projectSettingsFilePath == m_project->getProjectSettingsFilePath())
	{
		if (message->settingsChanged && m_hasGUI)
		{
			m_project->setStateSettingsUpdated();
			refreshProject(REFRESH_ALL_FILES);
		}
	}
	else
	{
		createAndLoadProject(projectSettingsFilePath);

		if (message->refreshMode != REFRESH_NONE)
		{
			refreshProject(message->refreshMode);
		}
	}
}

void Application::handleMessage(MessageRefresh* message)
{
	TRACE("app refresh");

	if (m_hasGUI && message->uiOnly)
	{
		if (message->loadStyle)
		{
			loadStyle(ApplicationSettings::getInstance()->getColorSchemePath());
		}

		m_mainView->refreshView();
		m_componentManager->refreshViews();
	}

	if (!message->uiOnly)
	{
		refreshProject(message->all ? REFRESH_ALL_FILES : REFRESH_UPDATED_FILES);
	}
}

void Application::handleMessage(MessageSwitchColorScheme* message)
{
	MessageStatus("Switch color scheme: " + message->colorSchemePath.str()).dispatch();

	loadStyle(message->colorSchemePath);
	MessageRefresh().refreshUiOnly().noReloadStyle().dispatch();
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

		LicenseChecker::LicenseState state = LicenseChecker::getInstance()->checkCurrentLicense();
		if (state != LicenseChecker::LICENSE_VALID && state != LicenseChecker::LICENSE_MOVED)
		{
			MessageForceEnterLicense(state).dispatch();
		}
	}
}

void Application::startMessagingAndScheduling()
{
	TaskScheduler::getInstance()->startSchedulerLoopThreaded();

	MessageQueue* queue = MessageQueue::getInstance().get();
	queue->addMessageFilter(std::make_shared<MessageFilterFocusInOut>());
	queue->addMessageFilter(std::make_shared<MessageFilterNewErrors>());
	queue->addMessageFilter(std::make_shared<MessageFilterSearchAutocomplete>());

	queue->setSendMessagesAsTasks(true);
	queue->startMessageLoopThreaded();
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
		while (recentProjects.size() > 7)
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
		std::string title = "Sourcetrail";

		switch (m_licenseType)
		{
			case MessageEnteredLicense::LICENSE_TEST:
				title += " [test]";
				break;
			case MessageEnteredLicense::LICENSE_NONE:
			case MessageEnteredLicense::LICENSE_NON_COMMERCIAL:
				title += " [non-commercial]";
				break;
			case MessageEnteredLicense::LICENSE_COMMERCIAL:
				break;
		}

		if (m_project)
		{
			FilePath projectPath = m_project->getProjectSettingsFilePath();

			if (!projectPath.empty())
			{
				title += " - " + projectPath.fileName();
			}
		}

		m_mainView->setTitle(title);
	}
}

bool Application::checkSharedMemory()
{
	std::string error = SharedMemory::checkSharedMemory(getUUID());
	if (error.size())
	{
		MessageStatus("Error on accessing shared memory. Indexing not possible. Please restart computer or run as admin: " + error, true).dispatch();
		handleDialog(
			"There was an error accessing shared memory on your computer: " + error + "\n\n"
			"Project indexing is not possible. Please restart your computer or try running Sourcetrail as admin. If the "
			"issue persists contact mail@sourcetrail.com");
		return false;
	}

	return true;
}
