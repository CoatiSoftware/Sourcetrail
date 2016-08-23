#include "utility/AppPath.h"
#include "utility/logging/ConsoleLogger.h"
#include "utility/logging/FileLogger.h"
#include "utility/logging/LogManager.h"
#include "utility/ResourcePaths.h"
#include "utility/ScopedFunctor.h"
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
}

int main(int argc, char *argv[])
{
	QApplication::setApplicationName("Coati Trial");

	if (QSysInfo::windowsVersion() != QSysInfo::WV_None)
	{
		QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
	}

	Version version = Version::fromString(GIT_VERSION_NUMBER);
	QApplication::setApplicationVersion(version.toDisplayString().c_str());

	setupPlatform(argc, argv);

	QtApplication qtApp(argc, argv);

	setupApp(argc, argv);
	init();

	qtApp.setAttribute(Qt::AA_UseHighDpiPixmaps);

	QtViewFactory viewFactory;
	QtNetworkFactory networkFactory;

	utility::loadFontsFromDirectory(ResourcePaths::getFontsPath(), ".otf");
	utility::loadFontsFromDirectory(ResourcePaths::getFontsPath(), ".ttf");

	Application::createInstance(version, &viewFactory, &networkFactory);
	ScopedFunctor f([](){
		Application::destroyInstance();
	});

	return qtApp.exec();
}
