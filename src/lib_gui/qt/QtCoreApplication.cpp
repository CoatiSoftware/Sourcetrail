#include "qt/QtCoreApplication.h"

#include <iostream>

#include "settings/ApplicationSettings.h"
#include "utility/file/FilePath.h"
#include "utility/logging/logging.h"
#include "utility/AppPath.h"
#include "utility/UserPaths.h"
#include "utility/utilityApp.h"
#include "License.h"

QtCoreApplication::QtCoreApplication(int argc, char **argv )
	: QCoreApplication(argc, argv)
{


}

QtCoreApplication::~QtCoreApplication()
{
}

void QtCoreApplication::handleMessage(MessageQuitApplication* message)
{
	emit quit();
}

void QtCoreApplication::handleMessage(MessageStatus* message)
{
//	if (m.substr(0,5) == "Index")
	{
		std::cout << message->status << std::endl;
	}
}

bool QtCoreApplication::saveLicense(License license)
{
	return utility::saveLicense(&license);
}
