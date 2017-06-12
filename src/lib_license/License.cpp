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

namespace
{
	std::string trimWhiteSpaces(const std::string &str)
	{
		auto wsfront = std::find_if_not(str.begin(), str.end(), [](int c){ return std::isspace(c); });
		auto wsback = std::find_if_not(str.rbegin(), str.rend(), [](int c){ return std::isspace(c); }).base();
		return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
	}
}


License::License()
	: m_rng(std::make_shared<Botan::AutoSeeded_RNG>())
{
    m_lines[BEGIN_LICENSE_LINE] = BEGIN_LICENSE;
    m_lines[SOURCETRAIL_LINE] = "Sourcetrail";
    m_lines[END_LICENSE_LINE] = END_LICENSE;
}

License::~License()
{
}

std::string License::getMessage() const
{
	std::string message = "";

    for (int i = OWNER_LINE; i < FIRST_SIGNATURE_LINE; ++i)
    {
        message += m_lines[i];
        if (m_lines[i].empty())
        {
            return "";
        }
    }

	return message;
}

std::string License::getSignature() const
{
	std::string signatue = "";

    if (!m_lines[LAST_SIGNATURE_LINE].empty())
	{
        for (int i = FIRST_SIGNATURE_LINE; i <= LAST_SIGNATURE_LINE; ++i)
		{
			signatue += m_lines[i];
		}
	}
	else
	{
		signatue = "";
	}

	return signatue;
}

std::string License::getLine(LICENSE_LINE line) const
{
    return m_lines[line];
}

void License::setLine(const LICENSE_LINE line, const std::string& value)
{
    // only allow to set message lines
    if (line < FIRST_SIGNATURE_LINE)
    {
        m_lines[line] = value;
    }
}

std::string License::getVersionLineWithoutPrefix() const
{
    return getLine(VERSION_LINE).substr(LicenseConstants::UNTIL_PREFIX.length());
}

unsigned int License::getSeats() const
{
    std::string line = getLine(SEATS_LINE);
    if (!line.empty())
    {
        try
        {
            return std::stoi(line);
        }
        catch (std::invalid_argument e)
        {
            return 0;
        }
    }
    return 0;
}

std::string License::getLicenseInfo() const
{
    std::string info = getLine(OWNER_LINE) + "\n";

    const std::string typeString = getLine(TYPE_LINE);
    info += typeString + "\n";
    info += getLine(VERSION_LINE) + "\n";

    // get info depending on license type
    if (isNonCommercialLicenseType(typeString))
    {
        info += "not registered for commercial development";
    }
    else if (typeString == LicenseConstants::TEST_LICENSE_STRING)
    {
        info += "unlimited Seats";
    }
    else if (getSeats() > 1)
    {
        info += std::to_string(getSeats()) + " Seats";
    }
    else
    {
        info += "1 Seat";
    }

    return info;
}

bool License::isNonCommercialLicenseType(const std::string type) const
{
    for ( const std::string nonCommercialLicenseType : NON_COMMERCIAL_LICENSE_TYPES)
    {
        if (type == nonCommercialLicenseType)
        {
            return true;
        }
    }
    return false;
}

int License::getTimeLeft() const
{
    const std::string dateText = "YYYY-MMM-DD";

    std::string dateString = getVersionLineWithoutPrefix();
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
	for(std::string line : m_lines)
	{
		licenseFile << line << std::endl;
	}
}

bool License::loadFromString(const std::string& licenseText)
{
	std::istringstream license(licenseText);
	return load(license);
}

bool License::load(std::istream& stream)
{
    int currentLine = 1;
	std::string line;

    while (getline(stream, line, '\n') && currentLine < END_LICENSE_LINE)
	{
        std::string l = trimWhiteSpaces(line);
        if (l == BEGIN_LICENSE)
        {
            continue;
        }
        if (l.size())
		{
            m_lines[currentLine] = l;
		}
        currentLine++;
	}

	if (m_lines.size() <= 0)
	{
		std::cout << "Could not read licence, possible empty string or only white spaces" << std::endl;
		return false;
	}

	return true;
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
	if(!m_publicKey)
	{
		std::cout << "No public key loaded" << std::endl;
		return false;
	}
    try
	{

		std::string signatureString = getSignature();
		if (signatureString.size() <= 0)
		{
			std::cout << "Could not read signature" << std::endl;
			return false;
		}

		Botan::secure_vector<Botan::byte> signature = Botan::base64_decode(signatureString);

		if (m_publicKey == NULL)
		{
			std::cout << "Public key is NULL" << std::endl;
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
		if (isExpired())
		{
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
    std::string expire = getVersionLineWithoutPrefix();
    if ( getLine(TYPE_LINE) == LicenseConstants::TEST_LICENSE_STRING)
    {
        return (getTimeLeft()==-1);
    }
    else
    {
        Version version = Version::fromShortString(expire);
        return Version::getApplicationVersion() > version;
    }
}

std::string License::getPublicKeyFilename() const
{
	if(m_publicKeyFilename.empty())
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

	if(boost::filesystem::exists(getPublicKeyFilename()))
	{
		Botan::RSA_PublicKey *rsaPublicKey = dynamic_cast<Botan::RSA_PublicKey *>(Botan::X509::load_key(getPublicKeyFilename()));

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

void License::setVersion(const std::string& version)
{
	if(!version.empty())
	{
		m_version = version;
	}
}

std::string License::getLicenseString() const
{
	std::stringstream license;
	for(std::string line : m_lines)
	{
		license << line << std::endl;
	}
	return license.str();
}

std::string License::hashLocation(const std::string& location)
{
	if (m_rng == NULL || location.size() <= 0)
	{
		return "";
	}

	return Botan::generate_passhash9(location, *(m_rng.get()));
}

bool License::checkLocation(const std::string& location, const std::string& hash)
{
	if (!location.size() || !hash.size())
	{
		return true;
	}

	return Botan::check_passhash9(location, hash);
}

std::string License::getLicenseEncodedString(const std::string& applicationLocation) const
{
	if (applicationLocation.size() <= 0)
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
		std::cout << "Failed to read licence string" << std::endl;
		return "";
	}

	std::string result = Botan::CryptoBox::encrypt(&fileContents[0], fileContents.size(), getEncodeKey(applicationLocation), rng);

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
	if (encodedLicense.size() <= 0)
	{
		std::cout << "No licence string given" << std::endl;
		return false;
	}

	if (applicationLocation.size() <= 0)
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
		loadFromString(Botan::CryptoBox::decrypt(crypbobxInput, getEncodeKey(applicationLocation))); // does the result of this operation matter at all? Is it supposed to throw if the input is wrong? wtf?
	}
	catch(...)
	{
		//loaded string from application settings is invalid
		return false;
	}

	return true;
}

std::string License::getHashedLicense() const
{
    return getEncodeKey("fakeKey");
}

std::string License::getEncodeKey(const std::string applicationLocation) const
{
	if (applicationLocation.size() <= 0)
	{
		std::cout << "No application location given" << std::endl;
		return "";
	}

	Botan::PBKDF *pbkdf = Botan::get_pbkdf("PBKDF2(SHA-256)");
	Botan::secure_vector<Botan::byte> salt = Botan::base64_decode("34zA54n60v4CxjY5n20k3J40c976n690", 32);
	Botan::AutoSeeded_RNG rng;
	return pbkdf->derive_key(32, applicationLocation, &salt[0], salt.size(), 10000).as_string();
}

void License::setSignature(const std::string& signature)
{
    std::stringstream ss;
    int line = FIRST_SIGNATURE_LINE;
    const int lineLength = 55;
    for (size_t i = 0; i < signature.size(); i++)
    {
        if(i % lineLength == 0 && i != 0)
        {
            m_lines[line++] = ss.str();
            ss.str("");
        }
        ss << signature[i];
    }

    m_lines[line] = ss.str();
}
