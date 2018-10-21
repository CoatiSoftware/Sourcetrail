#ifndef LICENSE_CHECKER_H
#define LICENSE_CHECKER_H

#include "MessageEnteredLicense.h"

class License;

class LicenseChecker
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

	static std::string getCurrentLicenseString();
	static void saveCurrentLicenseString(const std::string& licenseString);

	static bool isCurrentLicenseValid();
	static LicenseState checkCurrentLicense();
	static LicenseState checkLicenseString(const std::string& licenseString);

	static MessageEnteredLicense::LicenseType getCurrentLicenseType();
	static MessageEnteredLicense::LicenseType getLicenseType(const std::string& licenseString);

	static std::string getCurrentLicenseTypeString();

private:
	static LicenseState checkLicense(License& license);
};

#endif // LICENSE_CHECKER_H
