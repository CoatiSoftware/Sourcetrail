#include "includes.h"

#include "language_packages.h"

#include "LanguagePackageManager.h"
#include "InterprocessIndexer.h"
#include "ApplicationSettings.h"
#include "AppPath.h"
#include "ConsoleLogger.h"
#include "FileLogger.h"
#include "logging.h"
#include "LogManager.h"

#if BUILD_CXX_LANGUAGE_PACKAGE
#include "LanguagePackageCxx.h"
#endif // BUILD_CXX_LANGUAGE_PACKAGE

#if BUILD_JAVA_LANGUAGE_PACKAGE
#include "LanguagePackageJava.h"
#endif // BUILD_JAVA_LANGUAGE_PACKAGE

void setupLogging(const FilePath& logFilePath)
{
	LogManager* logManager = LogManager::getInstance().get();

	// std::shared_ptr<ConsoleLogger> consoleLogger = std::make_shared<ConsoleLogger>();
	// // consoleLogger->setLogLevel(Logger::LOG_WARNINGS | Logger::LOG_ERRORS);
	// consoleLogger->setLogLevel(Logger::LOG_ALL);
	// logManager->addLogger(consoleLogger);

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

	AppPath::setSharedDataPath(FilePath(appPath));
	UserPaths::setUserDataPath(FilePath(userDataPath));

	if (!logFilePath.empty())
	{
		setupLogging(FilePath(logFilePath));
	}

	suppressCrashMessage();

	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
	appSettings->load(FilePath(UserPaths::getAppSettingsPath()));
	LogManager::getInstance()->setLoggingEnabled(appSettings->getLoggingEnabled());

	LOG_INFO(L"sharedDataPath: " + AppPath::getSharedDataPath().wstr());
	LOG_INFO(L"userDataPath: " + UserPaths::getUserDataPath().wstr());


#if BUILD_CXX_LANGUAGE_PACKAGE
	LanguagePackageManager::getInstance()->addPackage(std::make_shared<LanguagePackageCxx>());
#endif // BUILD_CXX_LANGUAGE_PACKAGE

#if BUILD_JAVA_LANGUAGE_PACKAGE
	LanguagePackageManager::getInstance()->addPackage(std::make_shared<LanguagePackageJava>());
#endif // BUILD_JAVA_LANGUAGE_PACKAGE

	InterprocessIndexer indexer(instanceUuid, processId);
	indexer.work();

	return 0;
}
