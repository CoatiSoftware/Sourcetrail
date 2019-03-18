#ifndef LICENSE_GENERATOR_H
#define LICENSE_GENERATOR_H

#include <string>
#include <memory>

#include <botan/auto_rng.h>

#include "License.h"

namespace Botan
{
	class RSA_PrivateKey;
}

class License;

class LicenseGenerator
{
public:
	LicenseGenerator();
	~LicenseGenerator();

	void generatePrivateKey();
	bool loadPrivateKeyFromFile(const std::string& file);
	bool loadPrivateKeyFromString(const std::string& key);

	Botan::RSA_PrivateKey* getPrivateKey() const;

	std::string getPublicKeyPEMFileAsString();
	std::string getPrivateKeyPEMFileAsString();

	void writeKeysToFiles(const std::string& publicKeyFilename, const std::string& privateKeyFilename);

	std::unique_ptr<License> createLicenseByVersion(
		const std::string& user,
		const std::string& type,
		size_t numberOfUsers,
		const std::string& version
	);

	std::unique_ptr<License> createLicenseByQuarters(
		const std::string& user,
		const std::string& type,
		size_t numberOfUsers,
		size_t quarters
	);

	std::unique_ptr<License> createLicenseByDays(
		const std::string& user,
		const std::string& type,
		size_t numberOfUsers,
		size_t days
	);

	std::unique_ptr<License> createLicenseLifelong(
		const std::string& user,
		const std::string& type,
		size_t numberOfUsers
	);

private:
	bool createPrivateKey(Botan::RSA_PrivateKey* rsaPrivateKey);

	std::unique_ptr<License> createLicense(
		const std::string& user,
		const std::string& type,
		size_t numberOfUsers,
		const std::string& expirationVersion,
		const std::string& expirationDate
	);

	//Botan
	Botan::AutoSeeded_RNG m_rng;
	std::unique_ptr<Botan::RSA_PrivateKey> m_privateKey;
};

#endif // KEYGEN_H
