#include "Generator.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "boost/filesystem.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"
#include "botan/pk_keys.h"
#include "botan/pkcs8.h"
#include "botan/rsa.h"
#include "botan/pbkdf.h"
#include "botan/pubkey.h"
#include "botan/passhash9.h"
#include "botan/base64.h"
#include "botan/auto_rng.h"

#include "License.h"
#include "utility/Version.h"
#include "PrivateKey.h"
#include "PublicKey.h"

const std::string KEY_FILEENDING = ".pem";
const std::string PRIVATE_KEY_PASSWORD = "BA#jk5vbklAiKL9K3k$";
const char PRIVATE_KEY_FILE[] = "private-sourcetrail.pem";
const char PUBLIC_KEY_FILE[] = "public-sourcetrail.pem";

Generator::Generator()
{
    loadPrivateKeyFromString(PRIVATE_KEY);
}

Generator::~Generator()
{
}

void Generator::generateKeys()
{
    m_privateKey = std::unique_ptr<Botan::RSA_PrivateKey>(new Botan::RSA_PrivateKey(m_rng, 2048));
}

std::string Generator::getPrivateKeyFilename()
{
    if(m_privateKeyFile.empty())
    {
        return PRIVATE_KEY_FILE;
    }
    return m_privateKeyFile;
}

std::string Generator::getPublicKeyFilename()
{
    if(m_publicKeyFile.empty())
    {
        return PUBLIC_KEY_FILE;
    }
    return m_publicKeyFile;
}

std::string Generator::encodeLicense(const std::string& user, const int days)
{
	boost::gregorian::date today = boost::gregorian::day_clock::local_day();
	boost::gregorian::days daysToTry(days);
	boost::gregorian::date expireDate = today + daysToTry;

    createLicense(user, LicenseConstants::TEST_LICENSE_STRING, boost::gregorian::to_simple_string(expireDate), 0);

    return m_license->getLicenseString();
}

std::string Generator::encodeLicense(
    const std::string& user,
    const std::string& licenseType,
    const int seats,
    const std::string& version
)
{
    m_license = nullptr;
	if (user.size() <= 0)
	{
		std::cout << "No user given" << std::endl;
        return "";
	}

	if (licenseType.size() <= 0)
	{
		std::cout << "No licence type given" << std::endl;
        return "";
	}

    if (!version.empty())
    {
        Version tempVersion = Version::fromShortString(version);
        if (tempVersion.isValid())
        {
            createLicense(user, licenseType, tempVersion.toShortString(), seats);
        }
    }

    if (!m_license)
    {
        createLicense(user, licenseType, getExpireVersion(), seats);
    }


    return m_license->getLicenseString();
}

void Generator::printLicenseAndWriteItToFile()
{
    if (m_license)
    {
        m_license->print();
        m_license->writeToFile("license.txt");
    }
    else
    {
        std::cout << "nothing to print" << std::endl;
    }
}

bool Generator::verifyLicense(const std::string& filename)
{
    License license;
    license.loadFromFile(filename);
    license.loadPublicKeyFromString(PUBLIC_KEY);
    return license.isValid();
}

void Generator::setCustomPrivateKeyFile(const std::string& file)
{
    if(!file.empty())
    {
        m_privateKeyFile = file;
    }
}

void Generator::setCustomPublicKeyFile(const std::string& file)
{
    if(!file.empty())
    {
        m_publicKeyFile = file;
    }
}

std::string Generator::getPublicKeyPEMFileAsString()
{
    return Botan::X509::PEM_encode(*m_privateKey);
}

std::string Generator::getPrivateKeyPEMFileAsString()
{
	if (m_privateKey == NULL)
	{
		std::cout << "m_privateKey is NULL" << std::endl;
		return "";
	}

    return Botan::PKCS8::PEM_encode(*m_privateKey, m_rng, PRIVATE_KEY_PASSWORD);
}

void Generator::writeKeysToFiles()
{
    std::string publicKeyFilename = getPublicKeyFilename();
	if (publicKeyFilename.size() <= 0)
	{
		std::cout << "Failed to retrieve file name for public key" << std::endl;
		return;
	}

	std::string privateKeyFilename = getPrivateKeyFilename();
	if (privateKeyFilename.size() <= 0)
	{
		std::cout << "Failed to retrieve file name for private key" << std::endl;
		return;
	}


    std::cout << "public key filename: " << publicKeyFilename << std::endl;
    std::ofstream pub(publicKeyFilename);
    pub << getPublicKeyPEMFileAsString();
    std::cout << "public key created" << std::endl;

    std::cout << "private key filename: " << privateKeyFilename << std::endl;
    std::ofstream priv(privateKeyFilename);
    priv << getPrivateKeyPEMFileAsString();
    std::cout << "private key created" << std::endl;
}

bool Generator::loadPrivateKeyFromFile()
{
	if (boost::filesystem::exists(getPrivateKeyFilename()) == false)
	{
		return false;
	}

    Botan::Private_Key* privateKey = Botan::PKCS8::load_key(getPrivateKeyFilename(), m_rng, PRIVATE_KEY_PASSWORD);
    Botan::RSA_PrivateKey *rsaKey = dynamic_cast<Botan::RSA_PrivateKey *>(privateKey);
    
	if (!rsaKey)
	{
        std::cout << "The key is not a RSA key" << std::endl;
        return false;
    }

    m_privateKey = std::unique_ptr<Botan::RSA_PrivateKey>(rsaKey);

	return (m_privateKey != NULL);
}

bool Generator::loadPrivateKeyFromString(const std::string& key)
{
	if (key.size() <= 0)
	{
		std::cout << "No key string given" << std::endl;
		return false;
	}

    Botan::DataSource_Memory in(key);
    Botan::Private_Key* privateKey= Botan::PKCS8::load_key(in, m_rng, PRIVATE_KEY_PASSWORD);
    Botan::RSA_PrivateKey *rsaKey = dynamic_cast<Botan::RSA_PrivateKey *>(privateKey);
    
	if (!rsaKey)
	{
        std::cout << "The key is not a RSA key" << std::endl;
        return false;
    }

    m_privateKey = std::unique_ptr<Botan::RSA_PrivateKey>(rsaKey);

    return (m_privateKey != NULL);
}

Botan::RSA_PrivateKey *Generator::getPrivateKey() const
{
    return m_privateKey.get();
}

void Generator::createLicense(
    const std::string& user,
    const std::string& type,
    const std::string& expiration,
    const unsigned int seats
)
{
    m_license = std::unique_ptr<License>(new License());

    m_license->setLine(License::OWNER_LINE, user);
    m_license->setLine(License::TYPE_LINE, type);
    m_license->setLine(License::VERSION_LINE, LicenseConstants::UNTIL_PREFIX + expiration);
    if (seats > 1)
    {
        m_license->setLine(License::SEATS_LINE, std::to_string(seats) + " Seats");
    }
    else if (seats == 1)
    {
        m_license->setLine(License::SEATS_LINE, "1 Seat");
    }
    else
    {
        m_license->setLine(License::SEATS_LINE, "-");
    }
    Botan::AutoSeeded_RNG rng;
    std::string pass9 = Botan::generate_passhash9(m_license->getLine(License::VERSION_LINE), m_rng);
    m_license->setLine(License::HASH_LINE, pass9);

    //encode message
    const std::string emsa = "EMSA4(SHA-256)";
    Botan::PK_Signer signer(*(m_privateKey.get()), rng, emsa);
    Botan::DataSource_Memory in(m_license->getMessage());
    Botan::byte buffer[4096] = {0};

    while (size_t got = in.read(buffer, sizeof(buffer)))
    {
        signer.update(buffer, got);
    }

    const std::string signature = Botan::base64_encode(signer.signature(rng));
    m_license->setSignature(signature);
}

int Generator::mapMonthToVersion(int month)
{
    return (month-1)/3+1;
}

std::string Generator::getExpireVersion(int versions)
{
        boost::gregorian::date today = boost::gregorian::day_clock::local_day();
        int monthNumber = today.month().as_number();
        Version version(today.year(), mapMonthToVersion(monthNumber));
        version += versions;
        return version.toShortString();
}

