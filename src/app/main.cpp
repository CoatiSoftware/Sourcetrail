#include <memory>

#include <QApplication>
#include <QBitmap>
#include <QThread>
#include <QSplashScreen>
#include <QtWidgets/qmainwindow.h>

#include "Application.h"
#include "includes.h" // defines 'void setup(int argc, char *argv[])'
#include "qt/element/QtSplashScreen.h"
#include "qt/utility/utilityQt.h"
#include "qt/view/QtViewFactory.h"
#include "utility/logging/ConsoleLogger.h"
#include "utility/logging/FileLogger.h"
#include "utility/logging/LogManager.h"

void init()
{
	std::shared_ptr<ConsoleLogger> consoleLogger = std::make_shared<ConsoleLogger>();
	consoleLogger->setLogLevel(Logger::LOG_WARNINGS | Logger::LOG_ERRORS);
	LogManager::getInstance()->addLogger(consoleLogger);

	std::shared_ptr<FileLogger> fileLogger = std::make_shared<FileLogger>();
	fileLogger->setLogLevel(Logger::LOG_ALL);
	LogManager::getInstance()->addLogger(fileLogger);

	utility::loadFontsFromDirectory("data/fonts", ".otf");
}

class InitThread : public QThread
{
public:
	void run(void)
	{
		//min Time the Splash screen is displayed
		QThread::msleep(500);
	}
};

int main(int argc, char *argv[])
{
	setup(argc, argv);
	QApplication qtApp(argc, argv);

	QPixmap whitePixmap(500,500);
	whitePixmap.fill(Qt::white);

	QPixmap p;
	p.load("data/gui/splash_white.png");
	QPixmap foreground = p.scaled(whitePixmap.size()*0.8);
	p.load("data/gui/splash_blue.png");
	QPixmap background = p.scaled(whitePixmap.size()*0.9);

	QtSplashScreen* splash = new QtSplashScreen(whitePixmap, Qt::WindowStaysOnTopHint);
	qtApp.processEvents();
	splash->setForeground(foreground);
	splash->setBackground(background);

	splash->show();
	splash->repaint();

	qtApp.processEvents();
	if (splash) splash->message( "Starting GUI" );
	qtApp.processEvents();

	init();

	// Eventloop for Splashscreen
	QEventLoop loop;
	InitThread* initThread = new InitThread();
	QObject::connect(initThread, SIGNAL(finished()), &loop, SLOT(quit()));
	QObject::connect(initThread, SIGNAL(terminated()), &loop, SLOT(quit()));
	initThread->start();
	loop.exec();

	QtViewFactory viewFactory;
	std::shared_ptr<Application> app = Application::create(&viewFactory);

	if(splash)
	{
		delete splash;
	}

	return qtApp.exec();
}
