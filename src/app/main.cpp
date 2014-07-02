#include <memory>

#include <QApplication>

#include "Application.h"
#include "includes.h"
#include "qt/QtGuiFactory.h"

int main(int argc, char *argv[])
{
	QApplication qtApp(argc, argv);
	QtGuiFactory guiFactory;

	std::shared_ptr<Application> app = Application::create(&guiFactory);
	app->loadProject();

	return qtApp.exec();
}
