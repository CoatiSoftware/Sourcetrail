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

	static void createInstance();
	static std::shared_ptr<LicenseChecker> getInstance();

	~LicenseChecker() = default;

	std::string getCurrentLicenseString() const;
	void saveCurrentLicenseString(const std::string& licenseString) const;

	bool isCurrentLicenseValid();
	LicenseState checkCurrentLicense() const;
	LicenseState checkLicenseString(const std::string& licenseString) const;

	MessageEnteredLicense::LicenseType getCurrentLicenseType() const;
	MessageEnteredLicense::LicenseType getLicenseType(const std::string& licenseString) const;

private:
	LicenseChecker();
	LicenseChecker(const LicenseChecker&) = delete;
	void operator=(const LicenseChecker&) = delete;

	LicenseState checkLicense(License& license) const;

	static std::shared_ptr<LicenseChecker> s_instance;
};

#endif // LICENSE_CHECKER_H
