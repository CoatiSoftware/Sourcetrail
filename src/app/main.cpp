#include "includes.h" // defines 'void setup(int argc, char *argv[])'

#include <csignal>

#include "Application.h"
#include "data/indexer/IndexerFactory.h"
#include "data/indexer/IndexerFactoryModuleJava.h"
#include "data/indexer/IndexerFactoryModuleCxxCdb.h"
#include "data/indexer/IndexerFactoryModuleCxxManual.h"
#include "LicenseChecker.h"
#include "project/SourceGroupFactory.h"
#include "project/SourceGroupFactoryModuleCxx.h"
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
#include "utility/messaging/type/MessageInterruptTasks.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/ResourcePaths.h"
#include "utility/ScopedFunctor.h"
#include "utility/UserPaths.h"
#include "utility/utility.h"
#include "utility/utilityApp.h"
#include "utility/utilityPathDetection.h"
#include "utility/Version.h"
#include "version.h"

void signalHandler(int signum)
{
	std::cout << "interrupt running tasks" << std::endl;
	MessageInterruptTasks().dispatch();
}

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
			MessageStatus("Ran Java runtime path detection, found: " + paths.front().str()).dispatch();

			settings->setJavaPath(paths.front());
			settings->save();
		}
		else
		{
			MessageStatus("Ran Java runtime path detection, no path found.").dispatch();
		}
	}
}

void prefillJreSystemLibraryPaths()
{
	std::shared_ptr<ApplicationSettings> settings = ApplicationSettings::getInstance();
	if (settings->getJreSystemLibraryPaths().empty())
	{
		std::shared_ptr<CombinedPathDetector> jreSystemLibraryPathsDetector = utility::getJreSystemLibraryPathsDetector();
		std::vector<FilePath> paths = jreSystemLibraryPathsDetector->getPaths();
		if (!paths.empty())
		{
			MessageStatus("Ran JRE system library path detection, found: " + paths.front().str()).dispatch();

			settings->setJreSystemLibraryPaths(paths);
			settings->save();
		}
		else
		{
			MessageStatus("Ran JRE system library path detection, no path found.").dispatch();
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
			MessageStatus("Ran Maven executable path detection, found: " + paths.front().str()).dispatch();

			settings->setMavenPath(paths.front());
			settings->save();
		}
		else
		{
			MessageStatus("Ran Maven executable path detection, no path found.").dispatch();
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
			MessageStatus("Ran C/C++ header path detection, found " + std::to_string(paths.size()) + " paths").dispatch();

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
			MessageStatus("Ran C/C++ framework path detection, found " + std::to_string(paths.size()) + " paths").dispatch();

			settings->setFrameworkSearchPaths(paths);
			settings->save();
		}
	}
}

void prefillPaths()
{
	prefillJavaRuntimePath();
	prefillMavenExecutablePath();
	prefillJreSystemLibraryPaths();
	prefillCxxHeaderPaths();
	prefillCxxFrameworkPaths();
}

void addLanguageModules()
{
	SourceGroupFactory::getInstance()->addModule(std::make_shared<SourceGroupFactoryModuleCxx>());
	SourceGroupFactory::getInstance()->addModule(std::make_shared<SourceGroupFactoryModuleJava>());

	IndexerFactory::getInstance()->addModule(std::make_shared<IndexerFactoryModuleJava>());
	IndexerFactory::getInstance()->addModule(std::make_shared<IndexerFactoryModuleCxxCdb>());
	IndexerFactory::getInstance()->addModule(std::make_shared<IndexerFactoryModuleCxxManual>());
}

QCoreApplication* createApplication(int &argc, char *argv[], bool noGUI = false)
{
	if (noGUI)
	{
		return new QtCoreApplication(argc, argv);
	}
	else
	{
		return new QtApplication(argc, argv);
	}
}

int main(int argc, char *argv[])
{
	QApplication::setApplicationName("Sourcetrail");
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);

	Version version(
		VERSION_YEAR,
		VERSION_MINOR,
		VERSION_COMMIT,
		GIT_COMMIT_HASH
	);
	QApplication::setApplicationVersion(version.toDisplayString().c_str());

	MessageStatus(
		std::string("Starting Sourcetrail ") +
		(utility::getApplicationArchitectureType() == APPLICATION_ARCHITECTURE_X86_32 ? "32" : "64") + " bit, " +
		"version " + version.toDisplayString()).dispatch();

	commandline::CommandLineParser commandLineParser(version.toString());
	commandLineParser.preparse(argc, argv);
	if (commandLineParser.exitApplication())
	{
		return 0;
	}

	setupPlatform(argc, argv);

	if (commandLineParser.runWithoutGUI())
	{
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

		signal(SIGINT, signalHandler);
		signal(SIGTERM, signalHandler);
		signal(SIGABRT, signalHandler);

		commandLineParser.parse();
		if (commandLineParser.startedWithLicense())
		{
			utility::saveLicense(commandLineParser.getLicensePtr());
		}

		if (commandLineParser.exitApplication())
		{
			return 0;
		}

		if (!checker->isCurrentLicenseValid() && !ApplicationSettings::getInstance()->getNonCommercialUse())
		{
			std::string appName = argc > 0 && std::string(argv[0]).size() ? argv[0] : "sourcetrail";

			std::cout << "\nERROR: No valid license option selected.\n\n";
			std::cout << "For commercial use please run:\n\n";
			std::cout << "\t" << appName << " config --license-string <Commercial License or Test License string>\n";
			std::cout << "or\n";
			std::cout << "\t" << appName << " config --license-file <path/to/Commercial License or Test License file>\n\n\n";
			std::cout << "For non-commercial use please run:\n\n";
			std::cout << "\t" << appName << " config --non-commercial-use true\n" << std::endl;

			LOG_WARNING("Your current Sourcetrail license seems to be invalid. Please update your license info.");
			return 0;
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
#ifdef _WIN32
		{
			HWND consoleWnd = GetConsoleWindow();
			DWORD dwProcessId;
			GetWindowThreadProcessId(consoleWnd, &dwProcessId);
			if (GetCurrentProcessId() == dwProcessId)
			{
				// Sourcetrail has not been started from console and thus has it's own console
				ShowWindow(consoleWnd, SW_HIDE);
			}
		}
#endif
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
