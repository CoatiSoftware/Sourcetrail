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
	const std::string BEGIN_LICENSE_STRING = "-----BEGIN LICENSE-----";
	const std::string END_LICENSE_STRING = "-----END LICENSE-----";
    const std::string TEST_LICENSE_STRING = "Test License";
	const std::string PRODUCT_STRING = "Product: Sourcetrail";
	const std::string LICENSED_TO_STRING = "Licensed to: ";
	const std::string LICENSE_TYPE_STRING = "License type: ";
	const std::string VALID_UNTIL_STRING = "Valid until: ";
	const std::string VALID_UP_TO_STRING = "Valid up to version: ";
	const std::string SEPARATOR_STRING = "-";
    const int MINOR_VERSIONS_PER_YEAR = 4;
}

class License
{
public:
    enum LICENSE_LINE
    {
        BEGIN_LICENSE_LINE = 0,
		SOURCETRAIL_LINE = 1,
		USER_LINE = 2,
        TYPE_LINE = 3,
		EXPIRE_LINE = 4,
		SEPARATOR_LINE=5,
        HASH_LINE = 6,
        FIRST_SIGNATURE_LINE = 7,
        LAST_SIGNATURE_LINE = 13,
        END_LICENSE_LINE = 14,
        LICENSE_LINES = 15
    };

    License();
    ~License();

	std::string getMessage(bool withNewlines = false) const;
    std::string getSignature() const;
    std::string getVersionLineWithoutPrefix() const;
	std::string getLine(std::istream& stream);
	std::string getLineWithoutDescription(LICENSE_LINE line) const;

    void setLine(const LICENSE_LINE line, const std::string& value);
	void setHashLine(const std::string& hash);
	void setTypeAndSeats(const std::string& line);

    std::string getLicenseInfo() const;
    std::string getPublicKeyFilename() const;
    std::string getVersion() const;
	std::string getType() const;
	std::string getUser() const;
    unsigned int getSeats() const;

	void createHeader(
		const std::string& user,
		const std::string& type,
		const std::string& expiration,
		unsigned int seats = 0
	);
	std::string getExpireLine() const;

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
    void setSignature(const std::string&);

    bool isValid() const;
	bool isExpired() const;
	bool isTestLicense() const;

    void print();

    std::string hashLocation(const std::string&);
    static bool checkLocation(const std::string&, const std::string&);

private:
	std::string getEncodeKey(const std::string applicationLocation) const;
	bool extractData(const std::string& string, LICENSE_LINE line);
	std::string removeCaption(const std::string& line, const std::string& caption) const;

	bool isNonCommercialLicenseType() const;

	std::string m_publicKeyFilename;
	std::shared_ptr<Botan::RSA_PublicKey> m_publicKey;

	std::shared_ptr<Botan::AutoSeeded_RNG> m_rng;
	// message
	std::string m_user;
	std::string m_type;
	unsigned int m_seats;
	std::string m_expire;
	std::string m_hashLine;

	std::string m_signature;

    const std::string KEY_FILEENDING = ".pem";

    const std::vector<std::string> NON_COMMERCIAL_LICENSE_TYPES = { { "Private/Academic Single User License" } };
};

#endif // SOURCETRAIL_LICENSE_H
