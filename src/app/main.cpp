#include <memory>

#include <QApplication>

#include "Application.h"
#include "includes.h"
#include "qt/QtGuiFactory.h"
#include "qt/utility/utilityQt.h"

int main(int argc, char *argv[])
{
	QApplication qtApp(argc, argv);
	QtGuiFactory guiFactory;

	std::shared_ptr<Application> app = Application::create(&guiFactory);
	app->loadProject("data/ProjectSettings.xml");

	utility::loadFontsFromDirectory("data/fonts", ".otf");

	return qtApp.exec();
}
