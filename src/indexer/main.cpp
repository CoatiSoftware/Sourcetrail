#include "includes.h"

#include "LanguagePackageCxx.h"
#include "LanguagePackageJava.h"
#include "LanguagePackageManager.h"
#include "data/indexer/interprocess/InterprocessIndexer.h"
#include "settings/ApplicationSettings.h"
#include "utility/AppPath.h"
#include "utility/logging/ConsoleLogger.h"
#include "utility/logging/FileLogger.h"
#include "utility/logging/logging.h"
#include "utility/logging/LogManager.h"

void setupLogging(const FilePath& logFilePath)
{
	LogManager* logManager = LogManager::getInstance().get();

	std::shared_ptr<ConsoleLogger> consoleLogger = std::make_shared<ConsoleLogger>();
	// consoleLogger->setLogLevel(Logger::LOG_WARNINGS | Logger::LOG_ERRORS);
	consoleLogger->setLogLevel(Logger::LOG_ALL);
	logManager->addLogger(consoleLogger);

	std::shared_ptr<FileLogger> fileLogger = std::make_shared<FileLogger>();
	fileLogger->setLogFilePath(logFilePath);
	fileLogger->setLogLevel(Logger::LOG_ALL);
	logManager->addLogger(fileLogger);
}

void suppressCrashMessage()
{
#ifdef _WIN32
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#endif // _WIN32
}

int main(int argc, char *argv[])
{
	int processId = -1;
	std::string instanceUuid;
	std::string appPath;
	std::string userDataPath;
	std::string logFilePath;

	if (argc >= 2)
	{
		processId = std::stoi(argv[1]);
	}

	if (argc >= 3)
	{
		instanceUuid = argv[2];
	}

	if (argc >= 4)
	{
		appPath = argv[3];
	}

	if (argc >= 5)
	{
		userDataPath = argv[4];
	}

	if (argc >= 6)
	{
		logFilePath = argv[5];
	}

	AppPath::setAppPath(FilePath(appPath));
	UserPaths::setUserDataPath(FilePath(userDataPath));

	if (!logFilePath.empty())
	{
		setupLogging(FilePath(logFilePath));
	}

	suppressCrashMessage();

	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
	appSettings->load(FilePath(UserPaths::getAppSettingsPath()));
	LogManager::getInstance()->setLoggingEnabled(appSettings->getLoggingEnabled());

	LOG_INFO(L"appPath: " + AppPath::getAppPath().wstr());
	LOG_INFO(L"userDataPath: " + UserPaths::getUserDataPath().wstr());

	LanguagePackageManager::getInstance()->addPackage(std::make_shared<LanguagePackageCxx>());
	LanguagePackageManager::getInstance()->addPackage(std::make_shared<LanguagePackageJava>());

	InterprocessIndexer indexer(instanceUuid, processId);
	indexer.work();

	return 0;
}
