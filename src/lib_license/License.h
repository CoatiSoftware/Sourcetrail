#ifndef COATI_LICENSE_H
#define COATI_LICENSE_H

#include <memory>
#include <string>
#include <vector>

namespace Botan
{
	class RSA_PublicKey;
	class RSA_PrivateKey;
	class AutoSeeded_RNG;
}

class License
{
public:
    enum LicenseType
    {
        LICENSETYPE_SINGLEUSER = 0,
    };

    License();
    ~License();

    std::string getHashLine() const;
    std::string getMessage() const;
    std::string getSignature() const;
    std::string getVersionLine() const;
    std::string getOwnerLine() const;
    std::string getLicenseTypeLine() const;

    std::string getPublicKeyFilename() const;
    std::string getVersion() const;

    void create(std::string user, std::string version, Botan::RSA_PrivateKey* privateKey, unsigned int type = 0);

    std::string getLicenseString() const;

    void writeToFile(std::string filename);
    bool load(std::istream& stream);
    bool loadFromString(std::string licenseText);
    bool loadFromFile(std::string filename);

    bool loadPublicKeyFromFile(std::string);
    bool loadPublicKeyFromString(std::string);

    void setVersion(const std::string&);
    bool isValid() const;

    void print();

    std::string hashLocation(const std::string&);
    static bool checkLocation(const std::string&, const std::string&);

private:
    void createMessage(std::string user, std::string version, unsigned int type = 0);
    void addSignature(std::string);

    std::string m_version;
    std::string m_publicKeyFilename;
    std::shared_ptr<Botan::RSA_PublicKey> m_publicKey;
    std::vector<std::string> lines;
    std::shared_ptr<Botan::AutoSeeded_RNG> m_rng;

    const std::string KEY_FILEENDING = ".pem";
    const std::string BEGIN_LICENSE = "-----BEGIN LICENSE-----";
    const std::string END_LICENSE = "-----END LICENSE-----";
};

#endif // COATI_LICENSE_H
