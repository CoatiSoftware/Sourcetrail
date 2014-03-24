#include <QApplication>
#include <QTextEdit>

#include "utility.h"

int main(int argv, char **args)
{
	hello();

    QApplication app(argv, args);

    QTextEdit textEdit;
    textEdit.show();

    return app.exec();
}