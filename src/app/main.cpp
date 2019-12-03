#include "includes.h"

#include <csignal>
#include <iostream>

#include "language_packages.h"

#include "Application.h"
#include "ApplicationSettings.h"
#include "ApplicationSettingsPrefiller.h"
#include "CommandLineParser.h"
#include "ConsoleLogger.h"
#include "FileLogger.h"
#include "LanguagePackageManager.h"
#include "logging.h"
#include "LogManager.h"
#include "MessageIndexingInterrupted.h"
#include "MessageLoadProject.h"
#include "MessageStatus.h"
#include "productVersion.h"
#include "QtNetworkFactory.h"
#include "QtApplication.h"
#include "QtCoreApplication.h"
#include "QtViewFactory.h"
#include "ResourcePaths.h"
#include "ScopedFunctor.h"
#include "SourceGroupFactory.h"
#include "SourceGroupFactoryModuleCustom.h"
#include "UserPaths.h"
#include "utility.h"
#include "utilityApp.h"
#include "utilityQt.h"
#include "Version.h"

#if BUILD_CXX_LANGUAGE_PACKAGE
#include "LanguagePackageCxx.h"
#include "SourceGroupFactoryModuleCxx.h"
#endif // BUILD_CXX_LANGUAGE_PACKAGE

#if BUILD_JAVA_LANGUAGE_PACKAGE
#include "LanguagePackageJava.h"
#include "SourceGroupFactoryModuleJava.h"
#endif // BUILD_JAVA_LANGUAGE_PACKAGE

#if BUILD_PYTHON_LANGUAGE_PACKAGE
#include "SourceGroupFactoryModulePython.h"
#endif // BUILD_PYTHON_LANGUAGE_PACKAGE

void signalHandler(int signum)
{
	std::cout << "interrupt indexing" << std::endl;
	MessageIndexingInterrupted().dispatch();
}

void setupLogging()
{
	LogManager* logManager = LogManager::getInstance().get();

	std::shared_ptr<ConsoleLogger> consoleLogger = std::make_shared<ConsoleLogger>();
	consoleLogger->setLogLevel(Logger::LOG_ALL);
	logManager->addLogger(consoleLogger);

	std::shared_ptr<FileLogger> fileLogger = std::make_shared<FileLogger>();
	fileLogger->setLogDirectory(UserPaths::getLogPath().getAbsolute());
	fileLogger->setFileName(FileLogger::generateDatedFileName(L"log"));
	fileLogger->setLogLevel(Logger::LOG_ALL);
	fileLogger->deleteLogFiles(FileLogger::generateDatedFileName(L"log", L"", -30));
	logManager->addLogger(fileLogger);
}

void addLanguagePackages()
{
	SourceGroupFactory::getInstance()->addModule(std::make_shared<SourceGroupFactoryModuleCustom>());

#if BUILD_CXX_LANGUAGE_PACKAGE
	SourceGroupFactory::getInstance()->addModule(std::make_shared<SourceGroupFactoryModuleCxx>());
#endif // BUILD_CXX_LANGUAGE_PACKAGE

#if BUILD_JAVA_LANGUAGE_PACKAGE
	SourceGroupFactory::getInstance()->addModule(std::make_shared<SourceGroupFactoryModuleJava>());
#endif // BUILD_JAVA_LANGUAGE_PACKAGE

#if BUILD_PYTHON_LANGUAGE_PACKAGE
	SourceGroupFactory::getInstance()->addModule(std::make_shared<SourceGroupFactoryModulePython>());
#endif // BUILD_PYTHON_LANGUAGE_PACKAGE

#if BUILD_CXX_LANGUAGE_PACKAGE
	LanguagePackageManager::getInstance()->addPackage(std::make_shared<LanguagePackageCxx>());
#endif // BUILD_CXX_LANGUAGE_PACKAGE

#if BUILD_JAVA_LANGUAGE_PACKAGE
	LanguagePackageManager::getInstance()->addPackage(std::make_shared<LanguagePackageJava>());
#endif // BUILD_JAVA_LANGUAGE_PACKAGE
}

int main(int argc, char *argv[])
{
	QCoreApplication::addLibraryPath(".");

	if (utility::getOsType() == OS_LINUX && std::getenv("SOURCETRAIL_VIA_SCRIPT") == nullptr)
	{
		std::cout << "ERROR: Please run Sourcetrail via the Sourcetrail.sh script!" << std::endl;
	}

	QApplication::setApplicationName("Sourcetrail");

	if (utility::getOsType() != OS_LINUX)
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

	MessageStatus(
		std::wstring(L"Starting Sourcetrail ") +
		(utility::getApplicationArchitectureType() == APPLICATION_ARCHITECTURE_X86_32 ? L"32" : L"64") + L" bit, " +
		L"version " + version.toDisplayWString()
	).dispatch();

	commandline::CommandLineParser commandLineParser(version.toDisplayString());
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

		ApplicationSettingsPrefiller::prefillPaths(ApplicationSettings::getInstance().get());
		addLanguagePackages();

		signal(SIGINT, signalHandler);
		signal(SIGTERM, signalHandler);
		signal(SIGABRT, signalHandler);

		commandLineParser.parse();

		if (commandLineParser.exitApplication())
		{
			return 0;
		}

		if (commandLineParser.hasError())
		{
			std::wcout << commandLineParser.getError() << std::endl;
		}
		else
		{
			MessageLoadProject(
				commandLineParser.getProjectFilePath(),
				false,
				commandLineParser.getRefreshMode(),
				commandLineParser.getShallowIndexingRequested()
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

		ApplicationSettingsPrefiller::prefillPaths(ApplicationSettings::getInstance().get());
		addLanguagePackages();

		utility::loadFontsFromDirectory(ResourcePaths::getFontsPath(), L".otf");
		utility::loadFontsFromDirectory(ResourcePaths::getFontsPath(), L".ttf");

		if (commandLineParser.hasError())
		{
			Application::getInstance()->handleDialog(commandLineParser.getError());
		}
		else
		{
			MessageLoadProject(
				commandLineParser.getProjectFilePath(),
				false,
				REFRESH_NONE
			).dispatch();
		}

		return qtApp.exec();
	}
}
