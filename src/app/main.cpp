#include <memory>

#include <QApplication>
#include <QTextEdit>

#include "Application.h"
#include "includes.h"
#include "qt/QtElementFactory.h"

#include "data/parser/clang.h"

int main(int argv, char **args)
{
	clang_main(argv, args);

	QApplication qtApp(argv, args);
	std::shared_ptr<GuiElementFactory> elementFactory = std::make_shared<QtElementFactory>();

	std::shared_ptr<Application> app = Application::create(elementFactory);
	app->loadProject();

	QTextEdit textEdit;
	textEdit.show();

	return qtApp.exec();
}