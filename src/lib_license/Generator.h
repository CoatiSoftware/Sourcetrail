#ifndef KEYGEN_H
#define KEYGEN_H

#include <string>

#include "botan_all.h"

class Generator
{
public:
    Generator(std::string version = "x");
    ~Generator();

    std::string encodeLicense(std::string message, std::string licenseType);
    bool verifyLicense(std::string filename = "license.txt");
    void generateKeys();
    void writeKeysToFiles();
    void setVersion(std::string version);
    void setCustomPrivateKeyFile(std::string file);
    void setCustomPublicKeyFile(std::string file);

    std::string getPublicKeyPEMFileAsString();
    std::string getPrivateKeyPEMFileAsString();
    bool loadPrivateKeyFromFile();
    bool loadPrivateKeyFromString(std::string key);

    void PrintLicense();

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
