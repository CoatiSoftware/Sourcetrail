#ifndef LICENSE_CHECKER_H
#define LICENSE_CHECKER_H

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateTokenLocations.h"
#include "utility/messaging/type/MessageEnteredLicense.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageProjectNew.h"
#include "utility/messaging/type/MessageStatus.h"

#include "License.h"
#include "PublicKey.h"
#include "settings/ApplicationSettings.h"

#include "utility/AppPath.h"

class LicenseChecker
	: public MessageListener<MessageEnteredLicense>
	, public MessageListener<MessageLoadProject>
	, public MessageListener<MessageProjectNew>
	, public MessageListener<MessageActivateTokenLocations>
{
public:
	LicenseChecker()
		: m_forcedLicenseEntering(false)
	{
	}

	inline void setApp(Application* app)
	{
		m_app = app;
		if (!m_forcedLicenseEntering && !checkLicenseString())
		{
			m_app->showLicenseScreen();
			m_forcedLicenseEntering = true;
		}
	}

private:
	void handleMessage(MessageEnteredLicense* message)
	{
		m_forcedLicenseEntering = false;

		if (m_resendMessage)
		{
			m_resendMessage->dispatch();
			m_resendMessage.reset();
		}
	}

	void handleMessage(MessageLoadProject* message)
	{
		if (!checkLicenseString())
		{
			m_resendMessage = std::make_shared<MessageLoadProject>(*message);
			message->cancel();

			if (!m_forcedLicenseEntering)
			{
				m_app->showLicenseScreen();
				m_forcedLicenseEntering = true;
			}
		}
	}

	void handleMessage(MessageProjectNew* message)
	{
		if (!checkLicenseString())
		{
			m_resendMessage = std::make_shared<MessageProjectNew>(*message);
			message->cancel();

			if (!m_forcedLicenseEntering)
			{
				m_app->showLicenseScreen();
				m_forcedLicenseEntering = true;
			}
		}
	}

	void handleMessage(MessageActivateTokenLocations* message)
	{
		if (!checkLicenseString())
		{
			m_resendMessage = std::make_shared<MessageActivateTokenLocations>(*message);
			message->cancel();

			if (!m_forcedLicenseEntering)
			{
				m_app->showLicenseScreen();
				m_forcedLicenseEntering = true;
			}
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

			if (!License::checkLocation(p.absolute().str(), licenseCheck))
			{
				break;
			}

			std::string licenseString = appSettings->getLicenseString();
			if (licenseString.size() == 0)
			{
				break;
			}

			License license;
			bool isLoaded = license.loadFromEncodedString(licenseString,AppPath::getAppPath());
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
	std::shared_ptr<MessageBase> m_resendMessage;
	bool m_forcedLicenseEntering;
};

#endif // LICENSE_CHECKER_H
