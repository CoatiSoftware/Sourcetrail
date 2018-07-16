#include "License.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>

//#include "botan_all.h"
#include "boost/date_time.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/filesystem.hpp"
#include "botan/rsa.h"
#include "botan/cryptobox.h"
#include "botan/passhash9.h"
#include "botan/base64.h"
#include "botan/pubkey.h"
#include "botan/pk_keys.h"
#include "botan/auto_rng.h"
#include "botan/pbkdf.h"

#include "utility/Version.h"
#include "PublicKey.h"

namespace
{

std::string trim(const std::string &str)
{
	auto wsfront = std::find_if_not(str.begin(), str.end(), [](int c) { return std::isspace(c); });
	auto wsback = std::find_if_not(str.rbegin(), str.rend(), [](int c) { return std::isspace(c); }).base();
	return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
}

std::string toLowerCase(const std::string& in)
{
	std::string out;
	std::transform(in.begin(), in.end(), std::back_inserter(out), towlower);
	return out;
}

}

License::License()
	: m_rng(std::make_shared<Botan::AutoSeeded_RNG>())
	, m_user("")
	, m_type("Private License")
	, m_numberOfUsers(0)
	, m_createdWithSeats(false)
	, m_expire("")
{
	loadPublicKey();
}

License::~License()
{
}

void License::createHeader(
	const std::string& user,
	const std::string& type,
	const std::string& expiration,
	bool expiresAtDate,
	size_t numberOfUsers
)
{
	if (user.empty() || type.empty() || expiration.empty())
	{
		return;
	}
	m_user = user;
	m_expire = expiration;
	m_expiresAtDate = expiresAtDate;
	m_type = type;
	m_numberOfUsers = numberOfUsers;
}

std::string License::getMessage(bool withNewlines) const
{
	const std::string separator = (withNewlines ? "\n" : "");
	std::string message = "";

	if ( m_user.empty() || m_type.empty() || m_expire.empty() || m_hashLine.empty())
	{
		return "";
	}

	message += LicenseConstants::PRODUCT_STRING + separator;
	message += LicenseConstants::LICENSED_TO_STRING + m_user + separator;
	message += LicenseConstants::LICENSE_TYPE_STRING + m_type;

	if (m_numberOfUsers == 0)
	{
		message += (m_createdWithSeats ? " (unlimited seats)" : " (unlimited users)");
	}
	else if (m_numberOfUsers == 1)
	{
		message += (m_createdWithSeats ? " (1 Seat)" : " (1 user)");
	}
	else if (m_numberOfUsers > 1)
	{
		message += " (" + std::to_string(m_numberOfUsers) + (m_createdWithSeats ? " Seats)" : " users)");
	}

	message += separator;
	message += getExpireLine() + separator;
	message += LicenseConstants::SEPARATOR_STRING + separator;
	message += m_hashLine;

	return message;
}

std::string License::getLine(std::istream& stream)
{
	std::string line;
	if(getline(stream, line, '\n'))
	{
		return trim(line);
	}
	return "";
}

void License::setHashLine(const std::string &hash)
{
	if (!hash.empty())
	{
		m_hashLine = hash;
	}
}

std::string License::removeCaption(const std::string& line, const std::string& caption) const
{
	if (line.substr(0, caption.length()) == caption)
	{
		return line.substr(caption.length());
	}
	return "";
}

bool License::extractData(const std::string& data, LICENSE_LINE line)
{
	switch ( line )
	{
		case USER_LINE:
			m_user = removeCaption(data, LicenseConstants::LICENSED_TO_STRING);
			return !m_user.empty();
		case EXPIRE_LINE:
			m_expire = removeCaption(data, LicenseConstants::VALID_UP_TO_STRING);
			m_expiresAtDate = false;
			if (m_expire.empty())
			{
				m_expire = removeCaption(data, LicenseConstants::VALID_UNTIL_STRING);
				m_expiresAtDate = true;
			}
			return !m_expire.empty();
		case TYPE_LINE:
			setTypeAndNumberOfUsers(removeCaption(data, LicenseConstants::LICENSE_TYPE_STRING));
			return !m_type.empty();
		default:
			return false;
	}
}

bool License::isTestLicense() const
{
	return m_type == LicenseConstants::TEST_LICENSE_STRING;
}

bool License::isNonCommercialLicenseType() const
{
	for ( const std::string& nonCommercialLicenseType : NON_COMMERCIAL_LICENSE_TYPES)
	{
		if (m_type == nonCommercialLicenseType)
		{
			return true;
		}
	}
	return false;
}

size_t License::getNumberOfUsers() const
{
	return m_numberOfUsers;
}

std::string License::getType() const
{
	return m_type;
}

std::string License::getExpireLine() const
{
	return (m_expiresAtDate ?
		LicenseConstants::VALID_UNTIL_STRING :
		LicenseConstants::VALID_UP_TO_STRING) + m_expire;
}

std::string License::getExpireLineUI() const
{
	return toLowerCase(m_expiresAtDate ?
		LicenseConstants::VALID_UNTIL_STRING :
		LicenseConstants::VALID_UP_TO_STRING) + m_expire;
}

std::string License::getLicenseInfo() const
{
	std::string info = m_user + "\n";

	info += m_type + "\n";

	// get info depending on license type
	if (isNonCommercialLicenseType())
	{
		info += "not registered for commercial development\n";
	}
	else if (m_numberOfUsers == 0)
	{
		info += "unlimited users\n";
	}
	else if (m_numberOfUsers > 1)
	{
		info += std::to_string(m_numberOfUsers) + " users\n";
	}
	else
	{
		info += "1 user\n";
	}

	info += getExpireLineUI();

	return info;
}

std::string License::getUser() const
{
	return m_user;
}

int License::getTimeLeft() const
{
	const std::string dateText = "YYYY-MMM-DD";

	std::string dateString = m_expire;
	if (dateString.length() != dateText.length())
	{
		return -2;
	}

	boost::gregorian::date expireDate(
		boost::gregorian::from_simple_string(dateString));

	boost::gregorian::date today = boost::gregorian::day_clock::local_day();
	boost::gregorian::days daysLeft = expireDate - today;

	return (daysLeft.days() < 0 ? -1 : daysLeft.days());
}

void License::writeToFile(const std::string& filename)
{
	std::ofstream licenseFile(filename);
	licenseFile << getLicenseString() << std::endl;
}

bool License::loadFromString(const std::string& licenseText)
{
	std::istringstream license(licenseText);
	return load(license);
}

bool License::load(std::istream& stream)
{
	std::string line;
	std::array<std::string, LICENSE_LINES> lines;

	line = getLine(stream);
	if (line == LicenseConstants::BEGIN_LICENSE_STRING)
	{
		line = getLine(stream);
	}

	// sourcetrail line
	if (line != LicenseConstants::PRODUCT_STRING)
	{
		return false;
	}

	// user line
	if (!extractData(getLine(stream), USER_LINE))
	{
		return false;
	}

	if (!extractData(getLine(stream), TYPE_LINE))
	{
		return false;
	}

	// expire line
	if (!extractData(getLine(stream), EXPIRE_LINE))
	{
		return false;
	}

	// separator line
	getline(stream, line, '\n');
	if (trim(line) != LicenseConstants::SEPARATOR_STRING)
	{
		return false;
	}

	// hash line
	m_hashLine = getLine(stream);

	// signature
	m_signature = "";
	while (getline(stream, line, '\n'))
	{
		std::string l = trim(line);
		if (l == LicenseConstants::END_LICENSE_STRING)
		{
			break;
		}
		if (l.size())
		{
			m_signature += l;
		}
	}
	if (m_signature.length() != 344)
	{
		return false;
	}

	return true;
}

void License::setTypeAndNumberOfUsers(const std::string& line)
{
	size_t pos = line.find("(");

	if (pos != line.npos)
	{
		m_type = line.substr(0, pos - 1);
		try
		{
			m_numberOfUsers = std::stoi(line.substr(pos+1));
		}
		catch (std::invalid_argument e)
		{
			m_numberOfUsers = 0;
		}

		if (toLowerCase(line).find("seat") != line.npos)
		{
			m_createdWithSeats = true;
		}
	}
	else
	{
		m_type = line;
		m_numberOfUsers = 0;
	}
}

bool License::loadFromFile(const std::string& filename)
{
	std::ifstream sigfile(filename);
	return load(sigfile);
}

void License::print()
{
	std::cout << getLicenseString();
}

bool License::isValid() const
{
	if (!m_publicKey)
	{
		std::cout << "No public key loaded" << std::endl;
		return false;
	}
	try
	{
		if (m_signature.empty())
		{
			std::cout << "Could not read signature" << std::endl;
			return false;
		}

		Botan::secure_vector<Botan::byte> signature = Botan::base64_decode(m_signature);

		if (m_publicKey == NULL)
		{
			std::cout << "Public key is NULL" << std::endl;
			return false;
		}

		if (isExpired())
		{
			return false;
		}

		Botan::PK_Verifier verifier(*m_publicKey.get(), "EMSA4(SHA-256)");

		Botan::DataSource_Memory in(getMessage());
		Botan::byte buffer[4096] = {0};
		while(size_t got = in.read(buffer, sizeof(buffer)))
		{
			verifier.update(buffer, got); // what does 'got' stand for?
		}

		const bool ok = verifier.check_signature(signature);
		if (!ok)
		{
			std::cout << "License check failed" << std::endl;
			return false;
		}
		return ok;
	}
	catch(...) // invalid character, or something else... why not check what the exception is?
	{
		std::cout << "Invalid character in Licensekey" << std::endl;
	}
	return false;
}

bool License::isExpired() const
{
	if (m_expiresAtDate)
	{
		return (getTimeLeft() == -1);
	}
	else
	{
		Version version = Version::fromString(m_expire);
		return Version::getApplicationVersion().toShortVersion() > version;
	}
}

std::string License::getPublicKeyFilename() const
{
	if (m_publicKeyFilename.empty())
	{
		return "public-sourcetrail" + KEY_FILEENDING;
	}
	return m_publicKeyFilename;
}

bool License::loadPublicKeyFromFile(const std::string& filename)
{
	if (!filename.empty())
	{
		m_publicKeyFilename = filename;
	}

	if (boost::filesystem::exists(getPublicKeyFilename()))
	{
		Botan::RSA_PublicKey* rsaPublicKey =
			dynamic_cast<Botan::RSA_PublicKey*>(Botan::X509::load_key(getPublicKeyFilename()));

		if (!rsaPublicKey)
		{
			std::cout << "The loaded key is not a RSA key" << std::endl;
			return false;
		}

		m_publicKey = std::shared_ptr<Botan::RSA_PublicKey>(rsaPublicKey);
		return true;
	}

	return false;
}

bool License::loadPublicKey()
{
	Botan::DataSource_Memory in(PUBLIC_KEY);
	Botan::RSA_PublicKey *rsaPublicKey = dynamic_cast<Botan::RSA_PublicKey *>(Botan::X509::load_key(in));

	if (!rsaPublicKey)
	{
		std::cout << "The loaded key is not a RSA key" << std::endl;
		return false;
	}

	m_publicKey = std::shared_ptr<Botan::RSA_PublicKey>(rsaPublicKey);
	return true;
}

bool License::loadPublicKeyFromString(const std::string& publicKey)
{
	if (publicKey.empty())
	{
		std::cout << "Public key is empty" << std::endl;
		return false;
	}

	Botan::DataSource_Memory in(publicKey);
	Botan::RSA_PublicKey *rsaPublicKey = dynamic_cast<Botan::RSA_PublicKey *>(Botan::X509::load_key(in));

	if (!rsaPublicKey)
	{
		std::cout << "The loaded key is not a RSA key" << std::endl;
		return false;
	}

	m_publicKey = std::shared_ptr<Botan::RSA_PublicKey>(rsaPublicKey);
	return true;
}

std::string License::getLicenseString() const
{
	std::string license = "";
	license += LicenseConstants::BEGIN_LICENSE_STRING;
	license += "\n";
	license += getMessage(true) + "\n";
	license += getSignature() + "\n";
	license += LicenseConstants::END_LICENSE_STRING;
	license += "\n";

	return license;
}

std::string License::hashLocation(const std::string& location) const
{
	if (m_rng == NULL || location.size() <= 0)
	{
		return "";
	}

	return Botan::generate_passhash9(location, *(m_rng.get()));
}

bool License::checkLocation(const std::string& location, const std::string& hash)
{
	if (location.empty() || hash.empty())
	{
		return true;
	}

	return Botan::check_passhash9(location, hash);
}

std::string License::getLicenseEncodedString(const std::string& applicationLocation) const
{
	if (applicationLocation.empty())
	{
		std::cout << "No application location was given" << std::endl;
		return "";
	}

	Botan::AutoSeeded_RNG rng;
	std::vector<Botan::byte> fileContents;
	std::stringstream input(getLicenseString());

	//prepare the license string to work with the botan cryptobox
	while(input.good())
	{
		Botan::byte filebuffer[4096] = { 0 };
		input.read((char*)filebuffer, sizeof(filebuffer));
		size_t got = input.gcount(); // what does got stand for?

		fileContents.insert(fileContents.end(), filebuffer, filebuffer + got);
	}

	if(fileContents.size() <= 0)
	{
		std::cout << "Failed to read license string" << std::endl;
		return "";
	}

	std::string result =
		Botan::CryptoBox::encrypt(&fileContents[0], fileContents.size(), getEncodeKey(applicationLocation), rng);

	//remove Botan Cryptobox begin and end
	//should not be in the application settings
	const int leMagicNumberA = 40;
	const int leMagicNumberB = 78;

	if (result.length() < leMagicNumberA + leMagicNumberB)
	{
		std::cout << "Invalid result" << std::endl;
		return "";
	}

	result = result.substr(leMagicNumberA, result.length() - leMagicNumberB);

	return result;
}

bool License::loadFromEncodedString(const std::string& encodedLicense, const std::string& applicationLocation)
{
	if (encodedLicense.empty())
	{
		std::cout << "No license string given" << std::endl;
		return false;
	}

	if (applicationLocation.empty())
	{
		std::cout << "No application location given" << std::endl;
		return false;
	}

	try
	{
		//add cryptobox begin and end to loaded string
		std::string crypbobxInput = "-----BEGIN BOTAN CRYPTOBOX MESSAGE-----\n";
		crypbobxInput += encodedLicense;
		crypbobxInput += "-----END BOTAN CRYPTOBOX MESSAGE-----";

		//decrypt license
		return loadFromString(Botan::CryptoBox::decrypt(crypbobxInput, getEncodeKey(applicationLocation)));
	}
	catch(...)
	{
		//loaded string from application settings is invalid
		return false;
	}

	return false;
}

std::string License::getHashedLicense() const
{
	return getEncodeKey("fakeKey");
}

std::string License::getEncodeKey(const std::string applicationLocation) const
{
	if (applicationLocation.empty())
	{
		std::cout << "No application location given" << std::endl;
		return "";
	}

	Botan::PBKDF *pbkdf = Botan::get_pbkdf("PBKDF2(SHA-256)");
	Botan::secure_vector<Botan::byte> salt = Botan::base64_decode("34zA54n60v4CxjY5n20k3J40c976n690", 32);
	Botan::AutoSeeded_RNG rng;
	return pbkdf->derive_key(32, applicationLocation, &salt[0], salt.size(), 10000).as_string();
}

std::string License::getSignature() const
{
	std::string sig;
	std::stringstream ss;
	const int lineLength = 55;
	for (size_t i = 0; i < m_signature.size(); i++)
	{
		if(i % lineLength == 0 && i != 0)
		{
			sig += ss.str() + "\n";
			ss.str("");
		}
		ss << m_signature[i];
	}

	sig += ss.str();
	return sig;
}

void License::setSignature(const std::string& signature)
{
	m_signature = signature;
}
