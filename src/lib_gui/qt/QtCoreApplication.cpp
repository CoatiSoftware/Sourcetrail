#include "qt/QtCoreApplication.h"

#include <iostream>

#include "settings/ApplicationSettings.h"
#include "utility/file/FilePath.h"
#include "utility/logging/logging.h"
#include "utility/AppPath.h"
#include "utility/UserPaths.h"
#include "License.h"

QtCoreApplication::QtCoreApplication(int argc, char **argv )
	: QCoreApplication(argc, argv)
{
}

QtCoreApplication::~QtCoreApplication()
{
}

void QtCoreApplication::handleMessage(MessageFinishedParsing* message)
{
	std::cout << "Finished parsing" << std::endl;
	LOG_INFO("Finished parsing");
	this->quit();
}

bool QtCoreApplication::saveLicense(License license)
{
	if (license.isValid())
	{
		ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
		std::string appLocation = AppPath::getAppPath();
		appSettings->setLicenseString(license.getLicenseEncodedString(appLocation));
		FilePath p(appLocation);
		appSettings->setLicenseCheck(license.hashLocation(p.absolute().str()));
		appSettings->save(UserPaths::getAppSettingsPath());
		return true;
	}
	else
	{
		LOG_ERROR( "The entered license key is invalid.");
		return false;
	}
}
