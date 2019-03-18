#include "LicenseGenerator.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/filesystem.hpp>
#include <botan/auto_rng.h>
#include <botan/base64.h>
#include <botan/passhash9.h>
#include <botan/pbkdf.h>
#include <botan/pk_keys.h>
#include <botan/pkcs8.h>
#include <botan/pubkey.h>
#include <botan/rsa.h>

#include "Version.h"

const char PRIVATE_KEY_PASSWORD[] = "BA#jk5vbklAiKL9K3k$";

LicenseGenerator::LicenseGenerator()
{
}

LicenseGenerator::~LicenseGenerator()
{
}

void LicenseGenerator::generatePrivateKey()
{
	m_privateKey = std::make_unique<Botan::RSA_PrivateKey>(m_rng, 2048);
}

std::unique_ptr<License> LicenseGenerator::createLicenseByVersion(
	const std::string& user,
	const std::string& licenseType,
	size_t numberOfUsers,
	const std::string& version
){
	if (version.empty())
	{
		std::cout << "No version given" << std::endl;
		return nullptr;
	}

	Version tempVersion = Version::fromString(version);
	if (tempVersion.isValid())
	{
		return createLicense(user, licenseType, numberOfUsers, tempVersion.toShortString(), LicenseConstants::VALID_UNLIMITED);
	}

	return nullptr;
}

std::unique_ptr<License> LicenseGenerator::createLicenseByQuarters(
	const std::string& user,
	const std::string& type,
	size_t numberOfUsers,
	size_t quarters
){
	boost::gregorian::date today = boost::gregorian::day_clock::local_day();
	int quarter = (today.month().as_number() - 1) / 3 + 1;
	Version version(today.year(), quarter);
	version += quarters;

	return createLicenseByVersion(user, type, numberOfUsers, version.toShortString());
}

std::unique_ptr<License> LicenseGenerator::createLicenseByDays(
	const std::string& user,
	const std::string& type,
	size_t numberOfUsers,
	size_t days
){
	boost::gregorian::date today = boost::gregorian::day_clock::local_day();
	boost::gregorian::days daysToTry(days);
	boost::gregorian::date expireDate = today + daysToTry;

	return createLicense(
		user,
		(type.size() ? type : LicenseConstants::TEST_LICENSE), numberOfUsers,
		LicenseConstants::VALID_UNLIMITED,
		boost::gregorian::to_simple_string(expireDate)
	);
}

std::unique_ptr<License> LicenseGenerator::createLicenseLifelong(
	const std::string& user,
	const std::string& type,
	size_t numberOfUsers
){
	if (numberOfUsers == 0)
	{
		std::cout << "No user count given" << std::endl;
		return nullptr;
	}

	return createLicense(user, type, numberOfUsers, LicenseConstants::VALID_UNLIMITED, LicenseConstants::VALID_UNLIMITED);
}

std::string LicenseGenerator::getPublicKeyPEMFileAsString()
{
	if (!m_privateKey)
	{
		std::cout << "No private key loaded" << std::endl;
		return "";
	}

	return Botan::X509::PEM_encode(*m_privateKey);
}

std::string LicenseGenerator::getPrivateKeyPEMFileAsString()
{
	if (!m_privateKey)
	{
		std::cout << "No private key loaded" << std::endl;
		return "";
	}

	return Botan::PKCS8::PEM_encode(*m_privateKey, m_rng, PRIVATE_KEY_PASSWORD);
}

void LicenseGenerator::writeKeysToFiles(const std::string& publicKeyFilename, const std::string& privateKeyFilename)
{
	if (publicKeyFilename.size() <= 0)
	{
		std::cout << "Public key file path is empty" << std::endl;
		return;
	}

	if (privateKeyFilename.size() <= 0)
	{
		std::cout << "Private key file path is empty" << std::endl;
		return;
	}

	std::cout << "public key filename: " << publicKeyFilename << std::endl;
	std::ofstream pub(publicKeyFilename);
	pub << getPublicKeyPEMFileAsString();
	pub.close();
	std::cout << "public key created" << std::endl;

	std::cout << "private key filename: " << privateKeyFilename << std::endl;
	std::ofstream priv(privateKeyFilename);
	priv << getPrivateKeyPEMFileAsString();
	priv.close();
	std::cout << "private key created" << std::endl;
}

bool LicenseGenerator::loadPrivateKeyFromFile(const std::string& file)
{
	if (!boost::filesystem::exists(file))
	{
		std::cout << "Private key not found: " << file << std::endl;
		return false;
	}

	return createPrivateKey(dynamic_cast<Botan::RSA_PrivateKey *>(Botan::PKCS8::load_key(file, m_rng, PRIVATE_KEY_PASSWORD)));
}

bool LicenseGenerator::loadPrivateKeyFromString(const std::string& key)
{
	if (key.empty())
	{
		std::cout << "No key string given" << std::endl;
		return false;
	}

	Botan::DataSource_Memory in(key);
	return createPrivateKey(dynamic_cast<Botan::RSA_PrivateKey *>(Botan::PKCS8::load_key(in, m_rng, PRIVATE_KEY_PASSWORD)));
}

Botan::RSA_PrivateKey* LicenseGenerator::getPrivateKey() const
{
	return m_privateKey.get();
}

bool LicenseGenerator::createPrivateKey(Botan::RSA_PrivateKey* rsaPrivateKey)
{
	if (!rsaPrivateKey)
	{
		std::cout << "The key is not a RSA key" << std::endl;
		return false;
	}

	m_privateKey = std::unique_ptr<Botan::RSA_PrivateKey>(rsaPrivateKey);
	return true;
}

std::unique_ptr<License> LicenseGenerator::createLicense(
	const std::string& user,
	const std::string& type,
	size_t numberOfUsers,
	const std::string& expirationVersion,
	const std::string& expirationDate
){
	if (user.empty())
	{
		std::cout << "No user given" << std::endl;
		return nullptr;
	}

	if (type.empty())
	{
		std::cout << "No license type given" << std::endl;
		return nullptr;
	}

	if (expirationVersion.empty())
	{
		std::cout << "No expiration version given" << std::endl;
		return nullptr;
	}

	if (expirationDate.empty())
	{
		std::cout << "No expiration date given" << std::endl;
		return nullptr;
	}

	std::unique_ptr<License> license = std::make_unique<License>();
	license->setMessage(user, type, numberOfUsers, expirationVersion, expirationDate);

	Botan::AutoSeeded_RNG rng;

	// encode message
	const std::string emsa = "EMSA4(SHA-256)";
	Botan::PK_Signer signer(*(m_privateKey.get()), rng, emsa);
	Botan::DataSource_Memory in(license->getMessage(false));

	Botan::byte buffer[4096] = {0};
	while (size_t got = in.read(buffer, sizeof(buffer)))
	{
		signer.update(buffer, got);
	}

	const std::string signature = Botan::base64_encode(signer.signature(rng));
	license->setSignature(signature);

	return license;
}
