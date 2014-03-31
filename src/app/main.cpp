#include <memory>

#include <QApplication>
#include <QTextEdit>

#include "Application.h"
#include "qt/QtElementFactory.h"

int main(int argv, char **args)
{
	QApplication qtApp(argv, args);
	std::shared_ptr<GuiElementFactory> elementFactory = std::make_shared<QtElementFactory>();

	std::shared_ptr<Application> app = Application::create(elementFactory);
	app->loadProject();

	QTextEdit textEdit;
	textEdit.show();

	return qtApp.exec();
}