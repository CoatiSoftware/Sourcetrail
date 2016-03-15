#include "utility/AppPath.h"
#include "utility/logging/ConsoleLogger.h"
#include "utility/logging/FileLogger.h"
#include "utility/logging/LogManager.h"
#include "utility/ResourcePaths.h"
#include "utility/UserPaths.h"
#include "utility/Version.h"

#include "Application.h"
#include "includes.h" // defines 'void setup(int argc, char *argv[])'
#include "qt/network/QtNetworkFactory.h"
#include "qt/QtApplication.h"
#include "qt/utility/utilityQt.h"
#include "qt/view/QtViewFactory.h"
#include "qt/window/QtMainWindow.h"
#include "version.h"


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

int main(int argc, char *argv[])
{
	QApplication::setApplicationName("Coati Trial");

	Version version = Version::fromString(GIT_VERSION_NUMBER);
	QApplication::setApplicationVersion(version.toDisplayString().c_str());

	QtApplication qtApp(argc, argv);

	setup(argc, argv);

	qtApp.setAttribute(Qt::AA_UseHighDpiPixmaps);

	init();

	QtViewFactory viewFactory;
	QtNetworkFactory networkFactory;

	std::shared_ptr<Application> app = Application::create(version, &viewFactory, &networkFactory);

	return qtApp.exec();
}
