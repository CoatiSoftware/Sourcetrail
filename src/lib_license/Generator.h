#ifndef KEYGEN_H
#define KEYGEN_H

#include <string>

//#include "botan_all.h"
#include "botan/auto_rng.h"

namespace Botan
{
	class RSA_PrivateKey;
}

class Generator
{
public:
    Generator(const std::string& version = "x");
    ~Generator();

    std::string encodeLicense(const std::string& message, const std::string& licenseType);
	std::string encodeLicense(const std::string& message, const int days);
    bool verifyLicense(const std::string& filename = "license.txt");
    void generateKeys();
    void writeKeysToFiles();
    void setVersion(const std::string& version);
    void setCustomPrivateKeyFile(const std::string& file);
    void setCustomPublicKeyFile(const std::string& file);

    std::string getPublicKeyPEMFileAsString();
    std::string getPrivateKeyPEMFileAsString();
    bool loadPrivateKeyFromFile();
    bool loadPrivateKeyFromString(const std::string& key);

    // void PrintLicense(); // not implemented, is this deprecated or something?

    Botan::RSA_PrivateKey* getPrivateKey() const;

private:
    std::string getPrivateKeyFilename();
    std::string getPublicKeyFilename();

    //Botan
    Botan::AutoSeeded_RNG m_rng;

    std::string m_version;
    std::string m_privateKeyFile;
    std::string m_publicKeyFile;
    std::string m_license;
    std::shared_ptr<Botan::RSA_PrivateKey> m_privateKey;

    const std::string PRIVATE_KEY_PASSWORD = "BA#jk5vbklAiKL9K3k$";
    const std::string KEY_FILEENDING = ".pem";
};

#endif // KEYGEN_H
