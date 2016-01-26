#ifndef LICENSE_CHECKER_H
#define LICENSE_CHECKER_H

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageStatus.h"

#include "License.h"
#include "PublicKey.h"
#include "settings/ApplicationSettings.h"

#include "utility/AppPath.h"

class LicenseChecker
	: public MessageListener<MessageLoadProject>
{
public:
	inline void setApp(Application* app)
	{
		m_app = app;
		if (!checkLicenseString())
		{
			m_app->showLicenseScreen();
		}
	}

private:
	void handleMessage(MessageLoadProject* message)
	{
		if (!checkLicenseString())
		{
			message->cancel();
			m_app->showLicenseScreen();
		}
	}

	inline bool checkLicenseString()
	{
		MessageStatus("preparing...", false, true).dispatch();

		bool valid = false;
		do
		{
			ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

			std::string licenseCheck = appSettings->getLicenseCheck();
			std::string appPath = AppPath::getAppPath(); // for easier debugging...
			FilePath p(appPath);
			LOG_INFO_STREAM(<< "gonna check location: " << appPath);
			if (!License::checkLocation(p.absolute().str(), licenseCheck))
			{
				break;
			}
			LOG_INFO_STREAM(<< "öhm...? done checking?");

			std::string licenseString = appSettings->getLicenseString();
			if (licenseString.size() == 0)
			{
				break;
			}

			License license;
			bool isLoaded = license.loadFromString(licenseString);
			if (!isLoaded)
			{
				break;
			}

			license.loadPublicKeyFromString(PublicKey);
			valid = license.isValid();

		}
		while (false);

		MessageStatus("ready").dispatch();

		return valid;
	}

	Application* m_app;
};

#endif // LICENSE_CHECKER_H
