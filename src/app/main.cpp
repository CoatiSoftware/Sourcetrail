#include "includes.h" // defines 'void setup(int argc, char *argv[])'

#include "Application.h"
#include "data/indexer/IndexerFactory.h"
#include "data/indexer/IndexerFactoryModuleJava.h"
#include "data/indexer/IndexerFactoryModuleCxxCdb.h"
#include "data/indexer/IndexerFactoryModuleCxxManual.h"
#include "LicenseChecker.h"
#include "project/SourceGroupFactory.h"
#include "project/SourceGroupFactoryModuleC.h"
#include "project/SourceGroupFactoryModuleCpp.h"
#include "project/SourceGroupFactoryModuleJava.h"
#include "qt/network/QtNetworkFactory.h"
#include "qt/QtApplication.h"
#include "qt/QtCoreApplication.h"
#include "qt/utility/utilityQt.h"
#include "qt/view/QtViewFactory.h"
#include "settings/ApplicationSettings.h"
#include "utility/commandline/CommandLineParser.h"
#include "utility/logging/ConsoleLogger.h"
#include "utility/logging/FileLogger.h"
#include "utility/logging/LoggerUtility.h"
#include "utility/logging/logging.h"
#include "utility/logging/LogManager.h"
#include "utility/messaging/type/MessageEnteredLicense.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/ResourcePaths.h"
#include "utility/ScopedFunctor.h"
#include "utility/UserPaths.h"
#include "utility/utilityPathDetection.h"
#include "utility/Version.h"
#include "version.h"

void setupLogging()
{
	LogManager* logManager = LogManager::getInstance().get();

	std::shared_ptr<ConsoleLogger> consoleLogger = std::make_shared<ConsoleLogger>();
	// consoleLogger->setLogLevel(Logger::LOG_WARNINGS | Logger::LOG_ERRORS);
	consoleLogger->setLogLevel(Logger::LOG_ALL);
	logManager->addLogger(consoleLogger);

	std::shared_ptr<FileLogger> fileLogger = std::make_shared<FileLogger>();
	fileLogger->setLogDirectory(UserPaths::getLogPath());
	fileLogger->setFileName(LoggerUtility::generateDatedFileName("log"));
	fileLogger->setLogLevel(Logger::LOG_ALL);
	logManager->addLogger(fileLogger);
}

void prefillJavaRuntimePath()
{
	std::shared_ptr<ApplicationSettings> settings = ApplicationSettings::getInstance();
	if (settings->getJavaPath().empty())
	{
		std::shared_ptr<CombinedPathDetector> javaPathDetector = utility::getJavaRuntimePathDetector();
		std::vector<FilePath> paths = javaPathDetector->getPaths();
		if (!paths.empty())
		{
			MessageStatus("Run Java runtime path detection, found: " + paths.front().str());

			settings->setJavaPath(paths.front().str());
			settings->save();
		}
		else
		{
			MessageStatus("Run Java runtime path detection, no path found.");
		}
	}
}

void prefillMavenExecutablePath()
{
	std::shared_ptr<ApplicationSettings> settings = ApplicationSettings::getInstance();
	if (settings->getMavenPath().empty())
	{
		std::shared_ptr<CombinedPathDetector> mavenPathDetector = utility::getMavenExecutablePathDetector();
		std::vector<FilePath> paths = mavenPathDetector->getPaths();
		if (!paths.empty())
		{
			MessageStatus("Run Maven executable path detection, found: " + paths.front().str());

			settings->setMavenPath(paths.front());
			settings->save();
		}
		else
		{
			MessageStatus("Run Maven executable path detection, no path found.");
		}
	}
}

void prefillCxxHeaderPaths()
{
	std::shared_ptr<ApplicationSettings> settings = ApplicationSettings::getInstance();
	if (settings->getHeaderSearchPaths().empty())
	{
		std::shared_ptr<CombinedPathDetector> cxxHeaderDetector = utility::getCxxHeaderPathDetector();
		std::vector<FilePath> paths = cxxHeaderDetector->getPaths();
		if (!paths.empty())
		{
			MessageStatus("Run C/C++ header path detection, found " + std::to_string(paths.size()) + " paths");

			settings->setHeaderSearchPaths(paths);
			settings->save();
		}
	}
}

void prefillCxxFrameworkPaths()
{
	std::shared_ptr<ApplicationSettings> settings = ApplicationSettings::getInstance();
	if (settings->getFrameworkSearchPaths().empty())
	{
		std::shared_ptr<CombinedPathDetector> cxxFrameworkDetector = utility::getCxxFrameworkPathDetector();
		std::vector<FilePath> paths = cxxFrameworkDetector->getPaths();
		if (!paths.empty())
		{
			MessageStatus("Run C/C++ framework path detection, found " + std::to_string(paths.size()) + " paths");

			settings->setFrameworkSearchPaths(paths);
			settings->save();
		}
	}
}

void prefillPaths()
{
	prefillJavaRuntimePath();
	prefillMavenExecutablePath();
	prefillCxxHeaderPaths();
	prefillCxxFrameworkPaths();
}

void addLanguageModules()
{
	SourceGroupFactory::getInstance()->addModule(std::make_shared<SourceGroupFactoryModuleC>());
	SourceGroupFactory::getInstance()->addModule(std::make_shared<SourceGroupFactoryModuleCpp>());
	SourceGroupFactory::getInstance()->addModule(std::make_shared<SourceGroupFactoryModuleJava>());

	IndexerFactory::getInstance()->addModule(std::make_shared<IndexerFactoryModuleJava>());
	IndexerFactory::getInstance()->addModule(std::make_shared<IndexerFactoryModuleCxxCdb>());
	IndexerFactory::getInstance()->addModule(std::make_shared<IndexerFactoryModuleCxxManual>());
}

int main(int argc, char *argv[])
{
	QApplication::setApplicationName("Sourcetrail");

	if (QSysInfo::windowsVersion() != QSysInfo::WV_None)
	{
		QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
	}

    Version version(
		VERSION_YEAR,
		VERSION_MINOR,
        VERSION_COMMIT,
        GIT_COMMIT_HASH
    );
	QApplication::setApplicationVersion(version.toDisplayString().c_str());

	MessageStatus("Starting Sourcetrail " + version.toDisplayString()).dispatch();

	CommandLineParser commandLineParser(argc, argv, version.toString());
	if (commandLineParser.exitApplication())
	{
		return 0;
	}

	if (commandLineParser.runWithoutGUI())
	{
		setupPlatform(argc, argv);

		// headless Sourcetrail
		QtCoreApplication qtApp(argc, argv);

		setupApp(argc, argv);

		setupLogging();

		Application::createInstance(version, nullptr, nullptr);
		ScopedFunctor f([](){
			Application::destroyInstance();
		});

		prefillPaths();
		addLanguageModules();

		std::shared_ptr<LicenseChecker> checker = LicenseChecker::getInstance();

		if (commandLineParser.startedWithLicense())
		{
			qtApp.saveLicense(commandLineParser.getLicense());
			return 0;
		}

		if (!checker->isCurrentLicenseValid()) // this works because the user cannot enter a license string while running the app in headless more.
		{
			std::cout << "No or invalide License" << std::endl;
			LOG_WARNING("Your current Sourcetrail license seems to be invalid. Please update your license info.");
			return 0;
		}
		else
		{
			MessageEnteredLicense(checker->getCurrentLicenseType()).dispatch();
		}

		if (commandLineParser.hasError() )
		{
			std::cout << commandLineParser.getError() << std::endl;
		}
		else
		{
			MessageLoadProject(
				commandLineParser.getProjectFilePath(),
				commandLineParser.getFullProjectRefresh()
			).dispatch();
		}
		return qtApp.exec();
	}
	else
	{
		setupPlatform(argc, argv);

		QtApplication qtApp(argc, argv);

		setupApp(argc, argv);

		setupLogging();

		qtApp.setAttribute(Qt::AA_UseHighDpiPixmaps);

		QtViewFactory viewFactory;
		QtNetworkFactory networkFactory;

		Application::createInstance(version, &viewFactory, &networkFactory);
		ScopedFunctor f([](){
			Application::destroyInstance();
		});

		prefillPaths();
		addLanguageModules();

		utility::loadFontsFromDirectory(ResourcePaths::getFontsPath(), ".otf");
		utility::loadFontsFromDirectory(ResourcePaths::getFontsPath(), ".ttf");

		if (commandLineParser.hasError())
		{
			Application::getInstance()->handleDialog(commandLineParser.getError());
		}
		else
		{
			MessageLoadProject(
				commandLineParser.getProjectFilePath(),
				commandLineParser.getFullProjectRefresh()
			).dispatch();
		}

		return qtApp.exec();
	}
}
