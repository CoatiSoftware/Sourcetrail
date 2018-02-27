#ifndef KEYGEN_H
#define KEYGEN_H

#include <string>
#include <memory>

#include "botan/auto_rng.h"

namespace Botan
{
	class RSA_PrivateKey;
}
class License;

class Generator
{
public:
    Generator();
    ~Generator();

    std::string encodeLicense(
        const std::string& user,
        const std::string& licenseType,
        size_t numberOfUsers = 0,
        const std::string& version = ""
    );
    std::string encodeLicense(
        const std::string& user,
        const int days
    );
    void printLicenseAndWriteItToFile();
    bool verifyLicense(const std::string& filename = "license.txt");
    void generateKeys();
    void writeKeysToFiles();
    void setCustomPrivateKeyFile(const std::string& file);
    void setCustomPublicKeyFile(const std::string& file);

    std::string getPublicKeyPEMFileAsString();
    std::string getPrivateKeyPEMFileAsString();
    bool loadPrivateKeyFromFile();
    bool loadPrivateKeyFromString(const std::string& key);

    Botan::RSA_PrivateKey* getPrivateKey() const;
    void createLicense(
        const std::string& user,
		const std::string& type,
        const std::string& expiration,
        size_t numberOfUsers
    );

    std::string getExpireVersion(int versions = 4);
    void setVersionLine(int year, int minorVersion);
private:
    int mapMonthToVersion(int month);
    std::string getPrivateKeyFilename();
    std::string getPublicKeyFilename();

    //Botan
    Botan::AutoSeeded_RNG m_rng;

    std::string m_privateKeyFile;
    std::string m_publicKeyFile;
    std::unique_ptr<License> m_license;
    std::unique_ptr<Botan::RSA_PrivateKey> m_privateKey;

};

#endif // KEYGEN_H
