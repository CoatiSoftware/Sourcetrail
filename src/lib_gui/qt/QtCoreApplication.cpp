#include "qt/QtCoreApplication.h"

#include <iostream>

QtCoreApplication::QtCoreApplication(int argc, char **argv )
	: QCoreApplication(argc, argv)
{
}

QtCoreApplication::~QtCoreApplication()
{
}

void QtCoreApplication::handleMessage(MessageQuitApplication* message)
{
	std::cout << "quit" << std::endl;
	emit quit();
}

void QtCoreApplication::handleMessage(MessageStatus* message)
{
	std::cout << message->status << std::endl;
}
