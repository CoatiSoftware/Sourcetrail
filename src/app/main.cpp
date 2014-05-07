#include <memory>

#include <QApplication>
#include <QTextEdit>
#include <qlayout.h>
#include <qgroupbox.h>

#include "Application.h"
#include "includes.h"
#include "qt/element/QtArea.h"
#include "qt/element/QtCanvas.h"
#include "qt/QtElementFactory.h"
#include "qt/QtWindow.h"

int main(int argv, char **args)
{
	QApplication qtApp(argv, args);
	std::shared_ptr<GuiElementFactory> elementFactory = std::make_shared<QtElementFactory>();

	std::shared_ptr<QtWindow> window = std::make_shared<QtWindow>();

	std::shared_ptr<Application> app = Application::create(elementFactory, window);
	app->loadProject();

	return qtApp.exec();
}