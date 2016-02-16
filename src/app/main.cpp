#include "utility/AppPath.h"

#include <memory>

#include <QApplication>

#include "utility/logging/ConsoleLogger.h"
#include "utility/logging/FileLogger.h"
#include "utility/logging/LogManager.h"
#include "utility/StandardHeaderDetection.h"
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
#include "qt/window/QtSplashScreen.h"
#include "version.h"

#include "utility/solution/SolutionParserVisualStudio.h"
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

	utility::loadFontsFromDirectory(ResourcePaths::getFontsPath(), ".otf");
}

QMainWindow* getMainWindow()
{
	QWidgetList widgets = qApp->topLevelWidgets();
	for (QWidgetList::iterator i = widgets.begin(); i != widgets.end(); ++i)
		if ((*i)->objectName() == "MainWindow")
			return (QMainWindow*) (*i);
	return NULL;
}

int main(int argc, char *argv[])
{
	QApplication::setApplicationName("Coati");

	Version version = Version::fromString(GIT_VERSION_NUMBER);
	QApplication::setApplicationVersion(version.toDisplayString().c_str());

	setup(argc, argv);

	QtApplication qtApp(argc, argv);

	qtApp.setAttribute(Qt::AA_UseHighDpiPixmaps);

	QtSplashScreen* splash = new QtSplashScreen();
	splash->setMessage("Loading UI");
	splash->setVersion(version.toDisplayString().c_str());

//	QTimer* timer = new QTimer(splash);
//	QObject::connect(timer, SIGNAL(timeout()), splash, SLOT(animate()));
//	timer->start(150);

	splash->exec(qtApp);

	qtApp.processEvents();

	init();

	QtCommandLineParser commandLineParser;
	commandLineParser.setup();
	commandLineParser.process(qtApp);

	qtApp.processEvents();

	QtViewFactory viewFactory;
	QtNetworkFactory networkFactory;
	std::shared_ptr<Application> app = Application::create(version, &viewFactory, &networkFactory);

	if (AppPath::getAppPath().empty())
	{
		AppPath::setAppPath(QCoreApplication::applicationDirPath().toStdString());
	}

	if(ApplicationSettings::getInstance()->getHeaderSearchPaths().empty())
	{
		StandardHeaderDetection headerDetection;
		headerDetection.detectHeaders();
		ApplicationSettings::getInstance()->setHeaderSearchPaths(headerDetection.getDefaultHeaderPaths());
	}

	splash->setMessage("Checking License");
	LicenseChecker checker;
	checker.setApp(app.get());

	splash->setMessage("Parse Commandline Arguments");
	commandLineParser.parseCommandline();

	qtApp.processEvents();
	splash->setMessage("Done");

	if (splash)
	{
		delete splash;
	}

	return qtApp.exec();
}
