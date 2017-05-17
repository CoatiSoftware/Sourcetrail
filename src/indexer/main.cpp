#include "includes.h"

#include <QCoreApplication>

#include "data/indexer/IndexerFactory.h"
#include "data/indexer/IndexerFactoryModuleJava.h"
#include "data/indexer/IndexerFactoryModuleCxxCdb.h"
#include "data/indexer/IndexerFactoryModuleCxxManual.h"
#include "data/indexer/interprocess/InterprocessIndexer.h"
#include "settings/ApplicationSettings.h"
#include "utility/AppPath.h"
#include "utility/logging/ConsoleLogger.h"
#include "utility/logging/FileLogger.h"
#include "utility/logging/logging.h"
#include "utility/logging/LogManager.h"

void setupLogging(const std::string logFilePath)
{
	LogManager* logManager = LogManager::getInstance().get();

	std::shared_ptr<ConsoleLogger> consoleLogger = std::make_shared<ConsoleLogger>();
	// consoleLogger->setLogLevel(Logger::LOG_WARNINGS | Logger::LOG_ERRORS);
	consoleLogger->setLogLevel(Logger::LOG_ALL);
	logManager->addLogger(consoleLogger);

	std::shared_ptr<FileLogger> fileLogger = std::make_shared<FileLogger>();
	fileLogger->setLogFilePath(FilePath(logFilePath));
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
	QCoreApplication qtApp(argc, argv);

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

	AppPath::setAppPath(appPath);
	UserPaths::setUserDataPath(FilePath(userDataPath));

	setupLogging(logFilePath);
	suppressCrashMessage();

	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
	appSettings->load(FilePath(UserPaths::getAppSettingsPath()));
	LogManager::getInstance()->setLoggingEnabled(appSettings->getLoggingEnabled());

	LOG_INFO("appPath: " + appPath);
	LOG_INFO("userDataPath: " + userDataPath);

	IndexerFactory::getInstance()->addModule(std::make_shared<IndexerFactoryModuleJava>());
	IndexerFactory::getInstance()->addModule(std::make_shared<IndexerFactoryModuleCxxCdb>());
	IndexerFactory::getInstance()->addModule(std::make_shared<IndexerFactoryModuleCxxManual>());

	InterprocessIndexer indexer(instanceUuid, processId);
	indexer.work();

//	qtApp.quit();
	return 0;
}
