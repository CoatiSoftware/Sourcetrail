#include "Generator.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "boost/filesystem.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"
#include "botan/pk_keys.h"
#include "botan/pkcs8.h"
#include "botan/rsa.h"

#include "License.h"

Generator::Generator(const std::string& version)
    : m_version(version)
{
}

Generator::~Generator()
{
}

void Generator::generateKeys()
{
    m_privateKey = std::make_shared<Botan::RSA_PrivateKey>(m_rng, 2048);
}

std::string Generator::getPrivateKeyFilename()
{
    if(m_privateKeyFile.empty())
    {
        return "private-" + m_version + KEY_FILEENDING;
    }
    return m_privateKeyFile;
}

std::string Generator::getPublicKeyFilename()
{
    if(m_publicKeyFile.empty())
    {
        return "public-" + m_version + KEY_FILEENDING;
    }
    return m_publicKeyFile;
}

void Generator::setVersion(const std::string& version)
{
    if(!version.empty())
    {
        m_version = version;
    }
}

std::string Generator::encodeLicense(const std::string& user, const int days)
{
	boost::gregorian::date today = boost::gregorian::day_clock::local_day();
	boost::gregorian::days daysToTry(days);
	boost::gregorian::date expireDate = today + daysToTry;

	std::string testLicenseTypeString = "Test License - valid till " + boost::gregorian::to_simple_string(expireDate);
	return encodeLicense(user, testLicenseTypeString);
}

std::string Generator::encodeLicense(const std::string& user, const std::string& licenseType, const int seats)
{
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

    License license;

    //load private key
    std::string filename = getPrivateKeyFilename();

    std::shared_ptr<Botan::Private_Key> privateKey(
            Botan::PKCS8::load_key(filename, m_rng, PRIVATE_KEY_PASSWORD));

    Botan::RSA_PrivateKey *rsaKey = dynamic_cast<Botan::RSA_PrivateKey *>(privateKey.get());
    
	if (!rsaKey)
	{
        std::cout << "The key is not a RSA key" << std::endl;
    }

    license.create(user, m_version, rsaKey, licenseType, seats);
    license.writeToFile("license.txt");
    license.print();

    return license.getLicenseString();
}

bool Generator::verifyLicense(const std::string& filename)
{
    License license;
    license.loadFromFile(filename);
    license.setVersion(m_version);
    license.loadPublicKeyFromFile(getPublicKeyFilename());
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

    m_privateKey = std::shared_ptr<Botan::RSA_PrivateKey>(rsaKey);

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

    m_privateKey = std::shared_ptr<Botan::RSA_PrivateKey>(rsaKey);

    return (m_privateKey != NULL);
}

Botan::RSA_PrivateKey *Generator::getPrivateKey() const
{
    return m_privateKey.get();
}

