#ifndef LICENSE_CHECKER_H
#define LICENSE_CHECKER_H

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageEnteredLicense.h"
#include "utility/messaging/type/MessageDispatchWhenLicenseValid.h"

class License;

class LicenseChecker
	: public MessageListener<MessageDispatchWhenLicenseValid>
	, public MessageListener<MessageEnteredLicense>
{
public:
	enum LicenseState
	{
		LICENSE_EMPTY,
		LICENSE_MOVED,
		LICENSE_MALFORMED,
		LICENSE_INVALID,
		LICENSE_EXPIRED,
		LICENSE_VALID
	};

	static void createInstance();
	static std::shared_ptr<LicenseChecker> getInstance();

	~LicenseChecker();

	std::string getCurrentLicenseString() const;
	void saveCurrentLicenseString(const std::string& licenseString) const;

	bool isCurrentLicenseValid();
	LicenseState checkCurrentLicense() const;
	LicenseState checkLicenseString(const std::string licenseString) const;

private:
	LicenseChecker();
	LicenseChecker(const LicenseChecker&) = delete;
	void operator=(const LicenseChecker&) = delete;

	void handleMessage(MessageDispatchWhenLicenseValid* message);
	void handleMessage(MessageEnteredLicense* message);

	LicenseState checkLicense(License& license) const;

	static std::shared_ptr<LicenseChecker> s_instance;

	std::shared_ptr<MessageBase> m_pendingMessage;
	bool m_forcedLicenseEntering;
};

#endif // LICENSE_CHECKER_H
