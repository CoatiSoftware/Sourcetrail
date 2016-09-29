#include "Application.h"

#include "utility/logging/ConsoleLogger.h"
#include "utility/logging/FileLogger.h"
#include "utility/logging/logging.h"
#include "utility/logging/LogManager.h"
#include "utility/messaging/MessageQueue.h"
#include "utility/messaging/type/MessageDispatchWhenLicenseValid.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/messaging/type/MessageShowStartScreen.h"
#include "utility/scheduling/TaskScheduler.h"
#include "utility/tracing.h"
#include "utility/UserPaths.h"
#include "utility/Version.h"

#include "component/view/DialogView.h"
#include "component/view/GraphViewStyle.h"
#include "component/controller/NetworkFactory.h"
#include "component/controller/IDECommunicationController.h"
#include "component/view/MainView.h"
#include "component/view/ViewFactory.h"
#include "data/StorageCache.h"
#include "isTrial.h"
#include "LicenseChecker.h"
#include "settings/ApplicationSettings.h"
#include "settings/ColorScheme.h"

void Application::createInstance(
	const Version& version, ViewFactory* viewFactory, NetworkFactory* networkFactory
){
	Version::setApplicationVersion(version);
	loadSettings();

	TaskScheduler::getInstance();
	MessageQueue::getInstance();

	bool hasGui = (viewFactory != nullptr);
	s_instance = std::shared_ptr<Application>(new Application(hasGui));

	s_instance->m_storageCache = std::make_shared<StorageCache>();

	if (viewFactory != nullptr)
	{
		s_instance->m_componentManager = ComponentManager::create(viewFactory, s_instance->m_storageCache.get());

		s_instance->m_mainView = viewFactory->createMainView();
		s_instance->m_mainView->setTitle("Coati");

		MessageDispatchWhenLicenseValid(std::make_shared<MessageShowStartScreen>()).dispatch();

		s_instance->m_componentManager->setup(s_instance->m_mainView.get());
		s_instance->m_mainView->loadLayout();
	}

	if (networkFactory != nullptr)
	{
		s_instance->m_ideCommunicationController =
			networkFactory->createIDECommunicationController(s_instance->m_storageCache.get());
		s_instance->m_ideCommunicationController->startListening();
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

void Application::loadSettings()
{
	std::shared_ptr<ApplicationSettings> settings = ApplicationSettings::getInstance();
	settings->load(FilePath(UserPaths::getAppSettingsPath()));

	LogManager* logManager = LogManager::getInstance().get();
	if (!settings->getLoggingEnabled())
	{
		logManager->clearLoggers();
	}
	else if (!logManager->getLoggerCount())
	{
		std::shared_ptr<ConsoleLogger> consoleLogger = std::make_shared<ConsoleLogger>();
		consoleLogger->setLogLevel(Logger::LOG_WARNINGS | Logger::LOG_ERRORS);
		logManager->addLogger(consoleLogger);

		FileLogger::setFilePath(UserPaths::getLogPath());
		std::shared_ptr<FileLogger> fileLogger = std::make_shared<FileLogger>();
		fileLogger->setLogLevel(Logger::LOG_ALL);
		logManager->addLogger(fileLogger);
	}

	loadStyle(settings->getColorSchemePath());
}

void Application::loadStyle(const FilePath& colorSchemePath)
{
	ColorScheme::getInstance()->load(colorSchemePath);
	GraphViewStyle::loadStyleSettings();
}

std::shared_ptr<Application> Application::s_instance;

Application::Application(bool withGUI)
	: m_hasGUI(withGUI)
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
}

void Application::addProjectFactoryModule(std::shared_ptr<ProjectFactoryModule> module)
{
	m_projectFactory.addModule(module);
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

void Application::setTitle(const std::string& title)
{
	m_mainView->setTitle(title);
}

void Application::createAndLoadProject(const FilePath& projectSettingsFilePath)
{
	MessageStatus("Loading Project: " + projectSettingsFilePath.str(), false, true).dispatch();
	try
	{
		updateRecentProjects(projectSettingsFilePath);

		m_storageCache->clear();

		m_project = m_projectFactory.createProject(projectSettingsFilePath, m_storageCache.get(), getDialogView());

		if (m_project)
		{
			if (m_hasGUI)
			{
				setTitle("Coati - " + projectSettingsFilePath.fileName());
				m_mainView->hideStartScreen();

				m_componentManager->clearComponents();
				m_componentManager->refreshViews();
			}
		}
		else
		{
			LOG_ERROR_STREAM(<< "Failed to load project.");
			MessageStatus("Failed to load project.", true).dispatch();
		}
	}
	catch (...)
	{
		LOG_ERROR_STREAM(<< "Failed to load project.");
		MessageStatus("Failed to load project.", true).dispatch();
	}
}

void Application::refreshProject(bool force)
{
	bool indexing = m_project->refresh(force);
	if (indexing)
	{
		m_storageCache->clear();
		if (m_hasGUI)
		{
			m_componentManager->refreshViews();
		}
	}
}

void Application::handleMessage(MessageActivateWindow* message)
{
	if (m_hasGUI)
	{
		m_mainView->activateWindow();
	}
}

void Application::handleMessage(MessageFinishedParsing* message)
{
	m_project->logStats();
	if (m_hasGUI)
	{
		MessageRefresh().refreshUiOnly().dispatch();
	}
}

void Application::handleMessage(MessageLoadProject* message)
{
	TRACE("app load project");

	loadSettings();

	FilePath projectSettingsFilePath(message->projectSettingsFilePath);
	if (projectSettingsFilePath.empty())
	{
		return;
	}

	if (m_project && projectSettingsFilePath == m_project->getProjectSettingsFilePath())
	{
		if (message->forceRefresh)
		{
			m_project->setStateSettingsUpdated();
			refreshProject(false);
		}

		return;
	}

	createAndLoadProject(projectSettingsFilePath);
}

void Application::handleMessage(MessageRefresh* message)
{
	TRACE("app refresh");

	if (message->reloadSettings)
	{
		loadSettings();
	}

	if (message->uiOnly)
	{
		if (m_hasGUI)
		{
			m_componentManager->refreshViews();
		}
	}
	else
	{
		refreshProject(message->all);
	}
}

void Application::handleMessage(MessageSwitchColorScheme* message)
{
	loadStyle(message->colorSchemePath);
	MessageRefresh().refreshUiOnly().keepSettings().dispatch();
}

void Application::startMessagingAndScheduling()
{
	TaskScheduler::getInstance()->startSchedulerLoopThreaded();
	MessageQueue::getInstance()->setSendMessagesAsTasks(true);
	MessageQueue::getInstance()->startMessageLoopThreaded();
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

DialogView* Application::getDialogView() const
{
	if (m_componentManager)
	{
		return m_componentManager->getDialogView();
	}

	static DialogView dialogView;
	return &dialogView;
}
