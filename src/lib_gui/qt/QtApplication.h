#ifndef QT_APPLICATION_H
#define QT_APPLICATION_H

#include <QApplication>

class QtApplication
	: public QApplication
{
public:
	QtApplication(int& argc, char** argv);

	bool event(QEvent *event);
	int exec();
};

#endif // QT_APPLICATION_H
