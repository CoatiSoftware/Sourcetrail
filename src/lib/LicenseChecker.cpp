#include "LicenseChecker.h"

#include "utility/messaging/type/MessageStatus.h"

#include "License.h"
#include "Application.h"
#include "PublicKey.h"
#include "settings/ApplicationSettings.h"

#include "utility/AppPath.h"

void LicenseChecker::createInstance()
{
	if (!s_instance)
	{
		s_instance = std::shared_ptr<LicenseChecker>(new LicenseChecker());
	}
}

std::shared_ptr<LicenseChecker> LicenseChecker::getInstance()
{
	createInstance();

	return s_instance;
}

LicenseChecker::~LicenseChecker()
{
}

void LicenseChecker::setApp(Application* app)
{
	m_app = app;
}

LicenseChecker::LicenseChecker()
	: m_app(nullptr)
	, m_forcedLicenseEntering(false)
{
}

void LicenseChecker::handleMessage(MessageDispatchWhenLicenseValid* message)
{
	if (m_app != nullptr && !checkLicenseString())
	{
		m_pendingMessage = message->content;

		if (!m_forcedLicenseEntering)
		{
			m_app->forceEnterLicense();
			m_forcedLicenseEntering = true;
		}
	}
	else
	{
		message->content->dispatch();
	}
}

void LicenseChecker::handleMessage(MessageEnteredLicense* message)
{
	m_forcedLicenseEntering = false;

	if (m_pendingMessage)
	{
		m_pendingMessage->dispatch();
		m_pendingMessage.reset();
	}
}

bool LicenseChecker::checkLicenseString()
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
		if (license.isExpired())
		{
			valid = false;
		}

	}
	while (false);

	MessageStatus("ready").dispatch();

	return valid;
}

std::shared_ptr<LicenseChecker> LicenseChecker::s_instance;
