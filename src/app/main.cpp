#include <memory>

#include <QApplication>

#include "Application.h"
#include "includes.h"
#include "qt/QtElementFactory.h"
#include "qt/QtWindow.h"

int main(int argc, char *argv[])
{
	QApplication qtApp(argc, argv);
	std::shared_ptr<GuiElementFactory> elementFactory = std::make_shared<QtElementFactory>();

	std::shared_ptr<QtWindow> window = std::make_shared<QtWindow>();

	std::shared_ptr<Application> app = Application::create(elementFactory, window);
	app->loadProject();

	return qtApp.exec();
}
