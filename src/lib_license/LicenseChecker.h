#ifndef LICENSE_CHECKER_H
#define LICENSE_CHECKER_H

#include <string>
#include <memory>

#include "License.h"
#include "LicenseType.h"

namespace Botan
{
	class RSA_PublicKey;
}

class LicenseChecker
{
public:
	enum class LicenseState
	{
		EMPTY,
		INCOMPLETE,
		MALFORMED,
		INVALID,
		EXPIRED,
		VALID
	};

	static void setEncodeKey(const std::string& key);

	static bool loadPublicKeyFromFile(std::string fileName);
	static bool loadPublicKeyFromString(const std::string& publicKey);
	static bool loadPublicKey();

	static const License* getCurrentLicense();

	static std::string getCurrentLicenseString();
	static std::string getCurrentLicenseStringEncoded();

	static LicenseState setCurrentLicenseString(const std::string& licenseStr);
	static LicenseState setCurrentLicenseStringEncoded(const std::string& licenseStr);

	static LicenseState checkLicense(const License& license);
	static LicenseState checkLicenseString(const std::string& licenseString);
	static LicenseState checkCurrentLicense();

	static LicenseType getCurrentLicenseType();
	static std::string getCurrentLicenseTypeString();

	static std::string getLicenseErrorForState(LicenseState state);

private:
	static bool createPublicKey(Botan::RSA_PublicKey *rsaPublicKey);

	static std::string s_encodeKey;
	static Botan::RSA_PublicKey* s_publicKey; // Intentional memory leak, because destruction can cause crash
	static std::unique_ptr<License> s_currentLicense;
};

#endif // LICENSE_CHECKER_H
