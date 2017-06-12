#ifndef SOURCETRAIL_LICENSE_H
#define SOURCETRAIL_LICENSE_H

#include <memory>
#include <string>
#include <vector>
#include <array>

namespace Botan
{
	class RSA_PublicKey;
	class RSA_PrivateKey;
	class AutoSeeded_RNG;
}

namespace LicenseConstants {
    const std::string UNTIL_PREFIX = "Valid until ";
    const std::string TEST_LICENSE_STRING = "Test License";
    const int MINOR_VERSIONS_PER_YEAR = 4;
}

class License
{
public:
    enum LICENSE_LINE
    {
        BEGIN_LICENSE_LINE = 0,
        OWNER_LINE = 1,
        SOURCETRAIL_LINE = 2,
        TYPE_LINE = 3,
        VERSION_LINE = 4,
        SEATS_LINE=5,
        HASH_LINE = 6,
        FIRST_SIGNATURE_LINE = 7,
        LAST_SIGNATURE_LINE = 13,
        END_LICENSE_LINE = 14,
        LICENSE_LINES = 15
    };

    License();
    ~License();

    std::string getMessage() const;
    std::string getSignature() const;
    std::string getVersionLineWithoutPrefix() const;
    std::string getLine(LICENSE_LINE line) const;

    void setLine(const LICENSE_LINE line, const std::string& value);

    std::string getLicenseInfo() const;
    std::string getPublicKeyFilename() const;
    std::string getVersion() const;
    unsigned int getSeats() const;

	/// if Test License return >=0 or -1 if expired
	/// for non Test License -2
	int getTimeLeft() const;

    std::string getLicenseString() const;
	std::string getLicenseEncodedString(const std::string& applicationLocation) const;
    std::string getHashedLicense() const;

    void writeToFile(const std::string& filename);
    bool load(std::istream& stream);
    bool loadFromString(const std::string& licenseText);
    bool loadFromFile(const std::string& filename);
	bool loadFromEncodedString(const std::string& encodedLicense, const std::string& applicationLocation);

	bool loadPublicKeyFromFile(const std::string&);

    bool loadPublicKeyFromString(const std::string&);
	void setVersion(const std::string&);
    void setSignature(const std::string&);

    bool isValid() const;
	bool isExpired() const;

    void print();

    std::string hashLocation(const std::string&);
    static bool checkLocation(const std::string&, const std::string&);

private:
	void createMessage(const std::string& user, const std::string& version, const std::string& type = 0);
    void pushTodaysVersion();
    void pushVersionLine(int year, int minorVersion);
	std::string getEncodeKey(const std::string applicationLocation) const;

    bool isNonCommercialLicenseType(const std::string type) const;

	std::string m_version;
	std::string m_publicKeyFilename;
	std::shared_ptr<Botan::RSA_PublicKey> m_publicKey;
//	std::vector<std::string> m_lines;
    std::array<std::string, LICENSE_LINES> m_lines;
    std::shared_ptr<Botan::AutoSeeded_RNG> m_rng;

    const std::string KEY_FILEENDING = ".pem";
    const std::string BEGIN_LICENSE = "-----BEGIN LICENSE-----";
    const std::string END_LICENSE = "-----END LICENSE-----";

    const std::vector<std::string> NON_COMMERCIAL_LICENSE_TYPES = { { "Private/Academic Single User License" } };
};

#endif // SOURCETRAIL_LICENSE_H
