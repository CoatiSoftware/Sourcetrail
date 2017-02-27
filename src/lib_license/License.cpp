#include "License.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>
#include <istream>

//#include "botan_all.h"
#include "boost/date_time.hpp"
#include "boost/filesystem.hpp"
#include "botan/rsa.h"
#include "botan/cryptobox.h"
#include "botan/passhash9.h"
#include "botan/base64.h"
#include "botan/pubkey.h"
#include "botan/pk_keys.h"
#include "botan/auto_rng.h"
#include "botan/pbkdf.h"

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
}

License::~License()
{
}

std::string License::getHashLine() const
{
	if (m_lines.size() >= 5)
	{
		return m_lines[4];
	}
	else
	{
		return "";
	}
}

std::string License::getMessage() const
{
	std::string message = "";

	if (m_lines.size() >= 5)
	{
		for (int i = 1; i < 5; ++i)
		{
			message += m_lines[i];
		}
	}
	else
	{
		message = "";
	}

	return message;
}

std::string License::getSignature() const
{
	std::string signatue = "";

	if (m_lines.size() >= 12)
	{
		for (int i = 5; i < 12; ++i)
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

std::string License::getVersionLine() const
{
	if (m_lines.size() >= 3)
	{
		return m_lines[3];
	}
	else
	{
		return "";
	}
}

std::string License::getOwnerLine() const
{
	if (m_lines.size() >= 1)
	{
		return m_lines[1];
	}
	else
	{
		return "";
	}
}

std::string License::getLicenseTypeLine() const
{
	if (m_lines.size() >= 2)
	{
		return m_lines[2];
	}
	else
	{
		return "";
	}
}

uint License::getSeats() const
{
    std::string licenseTypeLine = getLicenseTypeLine();
    std::size_t found = licenseTypeLine.find(":");
    if (found != std::string::npos)
    {
        // +2 (": ")
        std::stringstream seatsStream(licenseTypeLine.substr(found+2));
        int seats;
        seatsStream>>seats;
        return seats;
    }
    return 0;
}

int License::getTimeLeft() const
{
	const std::string testText = "Test License - valid till ";
	const int leMagicNumber = 11;

	std::string licenceType = getLicenseTypeLine();

	if (licenceType.size() <= (testText.length() + leMagicNumber))
	{
		return -2; // well, since it's unclear what type of licence...??
	}

	if(licenceType.substr(0, testText.length()) == testText)
	{
		std::string dateString = licenceType.substr(testText.length(), leMagicNumber);

		boost::gregorian::date expireDate(
				boost::gregorian::from_simple_string(dateString));

		boost::gregorian::date today = boost::gregorian::day_clock::local_day();
		boost::gregorian::days daysLeft = expireDate - today;

		return (daysLeft.days() < 0 ? -1 : daysLeft.days());
	}
	else
	{
		return -2;
	}
}

void License::create(
		const std::string& user, const std::string& version,
        Botan::RSA_PrivateKey* privateKey, const std::string& type, const uint seats)
{
	m_version = version;
    if (seats > 0)
    {
        createMessage(user, version, type + ": " + std::to_string(seats) + " seats");
    }
    else
    {
        createMessage(user, version, type);
    }

	//encode message
	Botan::PK_Signer signer(*privateKey, *(m_rng.get()), "EMSA4(SHA-256)");
	Botan::DataSource_Memory in(getMessage());
	Botan::byte buffer[4096] = {0};

	while (size_t got = in.read(buffer, sizeof(buffer)))
	{
		signer.update(buffer, got); // the hell does 'got' stand for?
	}

	std::string signature = Botan::base64_encode(signer.signature(*(m_rng.get())));
	addSignature(signature);
}

void License::createMessage(const std::string& user, const std::string& version, const std::string& type)
{
	m_lines.clear();
	m_lines.push_back(BEGIN_LICENSE);
	m_lines.push_back(user);
	m_lines.push_back(type);
	std::string versionstring = "Coati " + getVersion();
	m_lines.push_back(versionstring);
	std::string pass9 = Botan::generate_passhash9(versionstring, *(m_rng.get()));
	m_lines.push_back(pass9);
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
	m_lines.clear();
	std::istringstream license(licenseText);
	return load(license);
}

bool License::load(std::istream& stream)
{
	m_lines.clear();
	std::string line;

	while (getline(stream, line, '\n'))
	{
		std::string l = trimWhiteSpaces(line);
		if (l.size())
		{
			m_lines.push_back(l);
		}
	}

	if (m_lines.size() <= 0)
	{
		std::cout << "Could not read licence, possible empty string or only white spaces" << std::endl;
		return false;
	}

	if (m_lines.front() != BEGIN_LICENSE)
	{
		std::cout << "No License Header" << std::endl;
		m_lines.insert(m_lines.begin(), BEGIN_LICENSE);
	}

	if (m_lines.back() != END_LICENSE)
	{
		std::cout << "No License Footer" << std::endl;
		m_lines.push_back(END_LICENSE);
	}

	if (m_lines.size() != 13)
	{
		return false;
	}

	return true;
}

bool License::loadFromFile(const std::string& filename)
{
	m_lines.clear();
	std::ifstream sigfile(filename);
	return load(sigfile);
}

void License::print()
{
	std::cout << getLicenseString();
}

void License::addSignature(const std::string& signature)
{
	if(m_lines.size() > 5)
	{
		std::cout << "signature already there" << std::endl;
		return;
	}
	if (signature.size() <= 0)
	{
		std::cout << "signature is empty." << std::endl;
		return;
	}
	std::stringstream ss;

	const int leMagicNumber = 55; // what does this number mean? signature length?
	for (size_t i = 0; i < signature.size(); i++)
	{
		if(i % leMagicNumber == 0 && i != 0)
		{
			m_lines.push_back(ss.str());
			ss.str("");
		}
		ss << signature[i];
	}

	m_lines.push_back(ss.str());
	m_lines.push_back(END_LICENSE); // why is this done here? the function name indicates only adding of signature
}

bool License::isValid() const
{
	if(!m_publicKey)
	{
		std::cout << "No public key loaded" << std::endl;
		return false;
	}
	try // lol, now we try to handle errors?
	{
		// why is this still here?
		if(Botan::check_passhash9("Coati "+ getVersion(), getHashLine()))
		{
	//        std::cout << "Hash from Coati "+ getVersion() + " confirmed" << std::endl;
		}

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
	return (getTimeLeft()==-1);
}

std::string License::getPublicKeyFilename() const
{
	if(m_publicKeyFilename.empty())
	{
		return "public-" + getVersion() + KEY_FILEENDING;
	}
	return m_publicKeyFilename;
}

std::string License::getVersion() const
{
	if(m_version.empty())
	{
		return "x";
	}
	return m_version;
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
	if (location.size() <= 0 || hash.size() <= 0)
	{
		return "";
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
