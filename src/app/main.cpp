#include "utility/AppPath.h"
#include "utility/commandline/CommandLineParser.h"
#include "utility/logging/ConsoleLogger.h"
#include "utility/logging/FileLogger.h"
#include "utility/logging/LogManager.h"
#include "utility/ResourcePaths.h"
#include "utility/UserPaths.h"
#include "utility/Version.h"

#include "Application.h"
#include "includes.h" // defines 'void setup(int argc, char *argv[])'
#include "LicenseChecker.h"
#include "qt/network/QtNetworkFactory.h"
#include "qt/QtApplication.h"
#include "qt/QtCoreApplication.h"
#include "qt/utility/utilityQt.h"
#include "qt/view/QtViewFactory.h"
#include "qt/window/QtMainWindow.h"
#include "version.h"

#include "settings/ProjectSettings.h"

void init()
{
	std::shared_ptr<ConsoleLogger> consoleLogger = std::make_shared<ConsoleLogger>();
	consoleLogger->setLogLevel(Logger::LOG_WARNINGS | Logger::LOG_ERRORS);
	LogManager::getInstance()->addLogger(consoleLogger);

	std::shared_ptr<FileLogger> fileLogger = std::make_shared<FileLogger>();
	fileLogger->setLogLevel(Logger::LOG_ALL);
	FileLogger::setFilePath(UserPaths::getLogPath());
	LogManager::getInstance()->addLogger(fileLogger);
}

void prepare(int argc, char *argv[])
{
	setup(argc, argv);

	init();
}

int main(int argc, char *argv[])
{
	QApplication::setApplicationName("Coati");

	Version version = Version::fromString(GIT_VERSION_NUMBER);
	QApplication::setApplicationVersion(version.toDisplayString().c_str());

	CommandLineParser commandLineParser(argc, argv, version.toString());
	if (commandLineParser.exitApplication())
	{
		return 0;
	}

	if (commandLineParser.runWithoutGUI())
	{
		// headless Coati
		QtCoreApplication qtApp(argc, argv);
		prepare(argc,argv);

		std::shared_ptr<Application> app = Application::create( version );

		if (commandLineParser.startedWithLicense())
		{
			qtApp.saveLicense(commandLineParser.getLicense());
			return 0;
		}

		commandLineParser.projectLoad();

		return qtApp.exec();
	}
	else
	{
		QtApplication qtApp(argc, argv);
		prepare(argc,argv);
		qtApp.setAttribute(Qt::AA_UseHighDpiPixmaps);

		QtViewFactory viewFactory;
		QtNetworkFactory networkFactory;

		LicenseChecker checker;

		utility::loadFontsFromDirectory(ResourcePaths::getFontsPath(), ".otf");
		std::shared_ptr<Application> app = Application::create(version, &viewFactory, &networkFactory);

		checker.setApp(app.get());

		commandLineParser.projectLoad();

		return qtApp.exec();
	}
}

