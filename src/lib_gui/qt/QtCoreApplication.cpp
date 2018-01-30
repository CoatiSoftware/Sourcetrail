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
	for (const std::wstring& status : message->stati())
	{
		std::wcout << status << std::endl;
	}
}
