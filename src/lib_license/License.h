#ifndef SOURCETRAIL_LICENSE_H
#define SOURCETRAIL_LICENSE_H

#include <memory>
#include <string>
#include <vector>

namespace Botan
{
	class AutoSeeded_RNG;
}

namespace LicenseConstants
{
	const char BEGIN_LICENSE[] = "-----BEGIN LICENSE-----";
	const char END_LICENSE[] = "-----END LICENSE-----";

	const char TEST_LICENSE[] = "Test License";

	const char PRODUCT[] = "Product: Sourcetrail";

	const char LICENSE_HOLDER[] = "License holder: ";
	const char LICENSE_TYPE[] = "License type: ";
	const char LICENSED_USERS[] = "Licensed number of users: ";
	const char LICENSED_VERSION[] = "Licensed product version: ";
	const char LICENSED_PERIOD[] = "Licensed usage period: ";

	const char VALID_UNTIL[] = "until ";
	const char VALID_UP_TO[] = "up to ";
	const char VALID_UNLIMITED[] = "unlimited";

	const char LICENSED_TO_OLD[] = "Licensed to: ";
	const char VALID_UNTIL_OLD[] = "Valid for: ";
	const char VALID_UP_TO_OLD[] = "Valid up to version: ";

	const char SEPARATOR[] = "-";
}

class License
{
public:
	License();
	~License();

	std::string getType() const;
	std::string getUser() const;
	size_t getNumberOfUsers() const;
	std::string getLicenseInfo() const;

	std::string getMessage(bool withNewlines) const;
	void setMessage(
		const std::string& user,
		const std::string& type,
		size_t numberOfUsers,
		const std::string& expirationVersion,
		const std::string& expirationDate
	);

	const std::string& getSignature() const;
	void setSignature(const std::string& signature);

	std::string getLicenseString() const;

	void writeToFile(const std::string& filename);

	bool loadFromString(const std::string& licenseText);
	bool loadFromFile(const std::string& filename);

	bool isEmpty() const;
	bool isComplete() const;
	bool isExpired() const;
	bool isTestLicense() const;

	// if Test License return >=0 or -1 if expired
	// for non Test License -2
	int getTimeLeft() const;

	void print();

	std::string generateHash(const std::string& str) const;
	static bool checkHash(const std::string& str, const std::string& hash);

private:
	bool load(std::istream& stream);
	bool loadFromLines(const std::vector<std::string>& lines);
	bool loadFromLinesOld(const std::vector<std::string>& lines);

	std::unique_ptr<Botan::AutoSeeded_RNG> m_rng;

	std::string m_user;
	std::string m_type;
	size_t m_numberOfUsers = 0;
	bool m_createdWithSeats = false;

	std::string m_expirationVersion;
	std::string m_expirationDate;

	std::string m_hashLine;
	std::string m_signature;

	bool m_isOldFormat = false;
};

#endif // SOURCETRAIL_LICENSE_H
