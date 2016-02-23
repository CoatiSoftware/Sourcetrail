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
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageFinishedParsing.h"


class Quitter : public MessageListener<MessageFinishedParsing>
{
public:
    Quitter(QCoreApplication* app){};
    virtual ~Quitter(){};
private:
    virtual void handleMessage(MessageFinishedParsing* message)
    {
        QCoreApplication::exit(0);
    }
};

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
	QApplication::setApplicationName("Coati");

	Version version = Version::fromString(GIT_VERSION_NUMBER);
	QApplication::setApplicationVersion(version.toDisplayString().c_str());
    QCoreApplication*  qtApp = new QCoreApplication(argc, argv);

	setup(argc, argv);
    if(AppPath::getAppPath().empty())
	{
		AppPath::setAppPath(QCoreApplication::applicationDirPath().toStdString());
	}

	init();

    QtCommandLineParser commandLineParser;
	commandLineParser.setup();
	commandLineParser.process(*qtApp);
    commandLineParser.evaluateCommandline();
    std::shared_ptr<Application> app;

    if(commandLineParser.noGUI())
    {
        app = Application::create( version );
        commandLineParser.projectLoad();

        Quitter quitter(qtApp);
        return qtApp->exec();
    }
    else
    {
        // replace the qt app with a gui qt app
        // FIXME qtApp to shared_ptr
        qtApp->quit();
        delete qtApp;
        qtApp = new QtApplication(argc, argv);
        qtApp->setAttribute(Qt::AA_UseHighDpiPixmaps);

        utility::loadFontsFromDirectory(ResourcePaths::getFontsPath(), ".otf");

        QtViewFactory viewFactory;
        QtNetworkFactory networkFactory;

        LicenseChecker checker;

        app = Application::create(version, &viewFactory, &networkFactory);

        checker.setApp(app.get());

		commandLineParser.projectLoad();
        return qtApp->exec();
    }
}

