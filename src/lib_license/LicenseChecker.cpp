#include "LicenseChecker.h"

#include <iostream>
#include <sstream>

#include <boost/filesystem.hpp>
// #include <botan_all.h>
#include <botan/auto_rng.h>
#include <botan/base64.h>
#include <botan/cryptobox.h>
#include <botan/pbkdf.h>
#include <botan/pk_keys.h>
#include <botan/pubkey.h>
#include <botan/rsa.h>

#include "License.h"
#include "PublicKey.h"

std::string LicenseChecker::s_encodeKey;
Botan::RSA_PublicKey* LicenseChecker::s_publicKey = nullptr;
std::unique_ptr<License> LicenseChecker::s_currentLicense;

void LicenseChecker::setEncodeKey(const std::string& key)
{
	if (key.empty())
	{
		return;
	}

	Botan::PBKDF *pbkdf = Botan::get_pbkdf("PBKDF2(SHA-256)");
	Botan::secure_vector<Botan::byte> salt = Botan::base64_decode("34zA54n60v4CxjY5n20k3J40c976n690", 32);
	s_encodeKey = pbkdf->derive_key(32, key, &salt[0], salt.size(), 10000).as_string();
}

bool LicenseChecker::loadPublicKeyFromFile(std::string fileName)
{
	if (!boost::filesystem::exists(fileName))
	{
		std::cout << "Public key file does not exist: " << fileName << std::endl;
		return false;
	}

	return createPublicKey(dynamic_cast<Botan::RSA_PublicKey*>(Botan::X509::load_key(fileName)));
}

bool LicenseChecker::loadPublicKeyFromString(const std::string& publicKey)
{
	if (publicKey.empty())
	{
		std::cout << "Public key is empty" << std::endl;
		return false;
	}

	Botan::DataSource_Memory in(publicKey);
	return createPublicKey(dynamic_cast<Botan::RSA_PublicKey *>(Botan::X509::load_key(in)));
}

bool LicenseChecker::loadPublicKey()
{
	return loadPublicKeyFromString(PUBLIC_KEY);
}

const License* LicenseChecker::getCurrentLicense()
{
	return s_currentLicense.get();
}

std::string LicenseChecker::getCurrentLicenseString()
{
	if (s_currentLicense)
	{
		return s_currentLicense->getLicenseString();
	}
	return "";
}

std::string LicenseChecker::getCurrentLicenseStringEncoded()
{
	if (!s_currentLicense)
	{
		std::cout << "No current license" << std::endl;
		return "";
	}

	if (s_encodeKey.empty())
	{
		std::cout << "No encode key" << std::endl;
		return "";
	}

	Botan::AutoSeeded_RNG rng;
	std::vector<Botan::byte> fileContents;
	std::stringstream input(s_currentLicense->getLicenseString());

	// prepare the license string to work with the botan cryptobox
	while (input.good())
	{
		Botan::byte filebuffer[4096] = { 0 };
		input.read((char*)filebuffer, sizeof(filebuffer));
		size_t got = input.gcount();

		fileContents.insert(fileContents.end(), filebuffer, filebuffer + got);
	}

	if (fileContents.size() <= 0)
	{
		std::cout << "Failed to read license string" << std::endl;
		return "";
	}

	std::string result = Botan::CryptoBox::encrypt(&fileContents[0], fileContents.size(), s_encodeKey, rng);

	// remove Botan Cryptobox begin and end
	// should not be in the application settings
	const int cryptoboxBeginLength = 40;
	const int cryptoboxEndLength = 38;

	if (result.length() < cryptoboxBeginLength + cryptoboxEndLength)
	{
		std::cout << "Invalid result" << std::endl;
		return "";
	}

	result = result.substr(cryptoboxBeginLength, result.length() - (cryptoboxBeginLength + cryptoboxEndLength));

	return result;
}

LicenseChecker::LicenseState LicenseChecker::setCurrentLicenseString(const std::string& licenseStr)
{
	s_currentLicense = nullptr;

	if (licenseStr.empty())
	{
		return LicenseState::EMPTY;
	}

	s_currentLicense = std::make_unique<License>();
	if (!s_currentLicense->loadFromString(licenseStr))
	{
		s_currentLicense = nullptr;
		return LicenseState::MALFORMED;
	}

	return checkCurrentLicense();
}

LicenseChecker::LicenseState LicenseChecker::setCurrentLicenseStringEncoded(const std::string& licenseStr)
{
	s_currentLicense = nullptr;

	if (licenseStr.empty())
	{
		return LicenseState::EMPTY;
	}

	if (s_encodeKey.empty())
	{
		std::cout << "No key available" << std::endl;
		return LicenseState::EMPTY;
	}

	s_currentLicense = std::make_unique<License>();

	try
	{
		// add cryptobox begin and end to loaded string
		std::string cryptoboxInput = "-----BEGIN BOTAN CRYPTOBOX MESSAGE-----\n";
		cryptoboxInput += licenseStr;
		cryptoboxInput += "-----END BOTAN CRYPTOBOX MESSAGE-----";

		// decrypt license
		if (!s_currentLicense->loadFromString(Botan::CryptoBox::decrypt(cryptoboxInput, s_encodeKey)))
		{
			s_currentLicense = nullptr;
			return LicenseState::MALFORMED;
		}
	}
	catch (...)
	{
		s_currentLicense = nullptr;
		return LicenseState::MALFORMED;
	}

	return checkCurrentLicense();
}

LicenseChecker::LicenseState LicenseChecker::checkLicense(const License& license)
{
	if (license.isEmpty())
	{
		return LicenseState::EMPTY;
	}

	if (!license.isComplete())
	{
		return LicenseState::INCOMPLETE;
	}

	std::string message = license.getMessage(false);
	std::string signature = license.getSignature();
	if (message.empty() || signature.empty())
	{
		return LicenseState::INCOMPLETE;
	}

	if (!s_publicKey)
	{
		std::cout << "No public key loaded" << std::endl;
		return LicenseState::INCOMPLETE;
	}

	try
	{
		Botan::PK_Verifier verifier(*s_publicKey, "EMSA4(SHA-256)");
		Botan::DataSource_Memory in(message);
		Botan::byte buffer[4096] = { 0 };

		while (size_t got = in.read(buffer, sizeof(buffer)))
		{
			verifier.update(buffer, got);
		}

		Botan::secure_vector<Botan::byte> sig = Botan::base64_decode(signature);
		if (!verifier.check_signature(sig))
		{
			return LicenseState::INVALID;
		}
	}
	catch (...)
	{
		std::cout << "Invalid character in Licensekey" << std::endl;
		return LicenseState::MALFORMED;
	}

	if (license.isExpired())
	{
		return LicenseState::EXPIRED;
	}

	return LicenseState::VALID;
}

LicenseChecker::LicenseState LicenseChecker::checkLicenseString(const std::string& licenseString)
{
	if (licenseString.size() == 0)
	{
		return LicenseState::EMPTY;
	}

	License license;
	bool isLoaded = license.loadFromString(licenseString);
	if (!isLoaded)
	{
		return LicenseState::MALFORMED;
	}

	return checkLicense(license);
}

LicenseChecker::LicenseState LicenseChecker::checkCurrentLicense()
{
	if (!s_currentLicense)
	{
		return LicenseState::EMPTY;
	}

	return checkLicense(*s_currentLicense.get());
}

LicenseType LicenseChecker::getCurrentLicenseType()
{
	if (!s_currentLicense)
	{
		return LicenseType::NON_COMMERCIAL;
	}

	LicenseState state = checkLicense(*s_currentLicense.get());
	if (state != LicenseState::VALID)
	{
		return LicenseType::NON_COMMERCIAL;
	}
	else if (s_currentLicense->isTestLicense())
	{
		return LicenseType::TEST;
	}

	return LicenseType::COMMERCIAL;
}

std::string LicenseChecker::getCurrentLicenseTypeString()
{
	// WARNING: Don't change these strings. The analytics API on the server relies on them.
	switch (getCurrentLicenseType())
	{
		case LicenseType::NON_COMMERCIAL:
			return "private";
		case LicenseType::TEST:
			return "test";
		case LicenseType::COMMERCIAL:
			return "commercial";
	}

	return "private";
}

std::string LicenseChecker::getLicenseErrorForState(LicenseState state)
{
	switch (state)
	{
		case LicenseState::EMPTY:
			return "No license key was entered.";
		case LicenseState::INCOMPLETE:
			return "The license key is incomplete.";
		case LicenseState::MALFORMED:
			return "The license key is malformed.";
		case LicenseState::INVALID:
			return "The license key is invalid.";
		case LicenseState::EXPIRED:
			return "The license key is expired.";
		case LicenseState::VALID:
			return "";
	}
}

bool LicenseChecker::createPublicKey(Botan::RSA_PublicKey *rsaPublicKey)
{
	if (!rsaPublicKey)
	{
		std::cout << "The loaded key is not a RSA key" << std::endl;
		return false;
	}

	s_publicKey = rsaPublicKey;
	return true;
}
