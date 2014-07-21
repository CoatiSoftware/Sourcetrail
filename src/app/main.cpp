#include <memory>

#include <QApplication>

#include "Application.h"
#include "includes.h" // defines 'void setup()'
#include "qt/QtGuiFactory.h"
#include "qt/utility/utilityQt.h"
#include "utility/logging/ConsoleLogger.h"
#include "utility/logging/LogManager.h"

void init()
{
	LogManager::getInstance()->addLogger(std::make_shared<ConsoleLogger>());

	utility::loadFontsFromDirectory("data/fonts", ".otf");
}

int main(int argc, char *argv[])
{
	setup();

	QApplication qtApp(argc, argv);
	QtGuiFactory guiFactory;

	init();

	std::shared_ptr<Application> app = Application::create(&guiFactory);
	app->loadProject("data/ProjectSettings.xml");

	return qtApp.exec();
}
