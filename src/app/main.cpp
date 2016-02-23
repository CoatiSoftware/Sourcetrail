#include "utility/AppPath.h"
#include "utility/logging/ConsoleLogger.h"
#include "utility/logging/FileLogger.h"
#include "utility/logging/LogManager.h"
#include "utility/ResourcePaths.h"
#include "utility/UserPaths.h"
#include "utility/Version.h"

#include "Application.h"
#include "includes.h" // defines 'void setup(int argc, char *argv[])'
#include "LicenseChecker.h"
#include "qt/commandline/QtCommandLineParser.h"
#include "qt/network/QtNetworkFactory.h"
#include "qt/QtApplication.h"
#include "qt/utility/utilityQt.h"
#include "qt/view/QtViewFactory.h"
#include "qt/window/QtMainWindow.h"
#include "version.h"

#include "settings/ProjectSettings.h"
#include "utility/solution/SolutionParserCompilationDatabase.h"

void init()
{
	std::shared_ptr<ConsoleLogger> consoleLogger = std::make_shared<ConsoleLogger>();
	consoleLogger->setLogLevel(Logger::LOG_WARNINGS | Logger::LOG_ERRORS);
	LogManager::getInstance()->addLogger(consoleLogger);

	std::shared_ptr<FileLogger> fileLogger = std::make_shared<FileLogger>();
	fileLogger->setLogLevel(Logger::LOG_ALL);
	FileLogger::setFilePath(UserPaths::getLogPath());
	LogManager::getInstance()->addLogger(fileLogger);

	utility::loadFontsFromDirectory(ResourcePaths::getFontsPath(), ".otf");
}
#include <iostream>

int main(int argc, char *argv[])
{
	QApplication::setApplicationName("Coati");

	Version version = Version::fromString(GIT_VERSION_NUMBER);
	QApplication::setApplicationVersion(version.toDisplayString().c_str());

	setup(argc, argv);
	QtApplication qtApp(argc, argv);

	qtApp.setAttribute(Qt::AA_UseHighDpiPixmaps);

	QPixmap whitePixmap(500, 500);
	whitePixmap.fill(Qt::white);

	init();

	QtCommandLineParser commandLineParser;
	commandLineParser.setup();
	commandLineParser.process(qtApp);

	QtViewFactory viewFactory;
	QtNetworkFactory networkFactory;

	if(AppPath::getAppPath().empty())
	{
		AppPath::setAppPath(QCoreApplication::applicationDirPath().toStdString());
	}

	LicenseChecker checker;

	std::shared_ptr<Application> app = Application::create(version, &viewFactory, &networkFactory);

	checker.setApp(app.get());

	commandLineParser.parseCommandline();

	return qtApp.exec();
}
