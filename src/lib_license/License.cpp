#include "License.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>
#include <istream>

#include "botan_all.h"
#include "boost/date_time.hpp"
#include "boost/filesystem.hpp"

namespace
{
    std::string trim(const std::string &str)
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
    return lines[4];
}

std::string License::getMessage() const
{
    std::string message = "";
    for(int i = 1; i < 5; ++i)
    {
        message += lines[i];
    }
    return message;
}

std::string License::getSignature() const
{
    std::string signatue = "";
    for(int i = 5; i < 12; ++i)
    {
        signatue += lines[i];
    }
    return signatue;
}

std::string License::getVersionLine() const
{
    return lines[3];
}

std::string License::getOwnerLine() const
{
    return lines[1];
}

std::string License::getLicenseTypeLine() const
{
    return lines[2];
}

int License::getTimeLeft() const
{
	const std::string testText = "Test License - valid till ";
	if(getLicenseTypeLine().substr(0, testText.length()) == testText)
	{
		boost::gregorian::date expireDate(
			boost::gregorian::from_simple_string(getLicenseTypeLine().substr(testText.length(),11))
		);
		boost::gregorian::date today = boost::gregorian::day_clock::local_day();
		boost::gregorian::days daysLeft = expireDate - today;
		return (daysLeft.days() < 0 ? 0 : daysLeft.days());
	}
	else
	{
		return -1;
	}
}

void License::create(
		const std::string& user, const std::string& version,
		Botan::RSA_PrivateKey* privateKey, const std::string& type)
{
    m_version = version;
    createMessage(user, version, type);

    //encode message
    Botan::PK_Signer signer(*privateKey, "EMSA4(SHA-256)");
    Botan::DataSource_Memory in(getMessage());
    Botan::byte buf[4096] = {0};
    while (size_t got = in.read(buf, sizeof(buf))) {
        signer.update(buf, got);
    }
	std::string signature = Botan::base64_encode(signer.signature(*(m_rng.get())));
    addSignature(signature);
}

void License::createMessage(const std::string& user, const std::string& version, const std::string& type)
{
    lines.clear();
    lines.push_back(BEGIN_LICENSE);
    lines.push_back(user);
    lines.push_back(type);
    std::string versionstring = "Coati " + getVersion();
    lines.push_back(versionstring);
	std::string pass9 = Botan::generate_passhash9(versionstring, *(m_rng.get()));
    lines.push_back(pass9);
}

void License::writeToFile(const std::string& filename)
{
    std::ofstream licenseFile(filename);
    for(std::string line : lines)
    {
        licenseFile << line << std::endl;
    }
}

bool License::loadFromString(const std::string& licenseText)
{
    lines.clear();
    std::istringstream license(licenseText);
    return load(license);
}

bool License::load(std::istream& stream)
{
    lines.clear();
    std::string line;

    while (getline(stream, line, '\n'))
    {
        std::string l = trim(line);
        if (l.size())
        {
            lines.push_back(l);
        }
    }

    if (lines.front() != BEGIN_LICENSE)
    {
        std::cout << "No License Header" << std::endl;
        lines.insert(lines.begin(), BEGIN_LICENSE);
    }

    if (lines.back() != END_LICENSE)
    {
        std::cout << "No License Footer" << std::endl;
        lines.push_back(END_LICENSE);
    }

    if (lines.size() != 13)
    {
        return false;
    }

    return true;
}

bool License::loadFromFile(const std::string& filename)
{
    lines.clear();
    std::ifstream sigfile(filename);
    return load(sigfile);
}

void License::print()
{
    std::cout << getLicenseString();
}

void License::addSignature(const std::string& signature)
{
    if(lines.size() > 5)
    {
        std::cout << "signature already there" << std::endl;
        return;
    }
    if (!signature.size()) {
        std::cout << "signature is empty." << std::endl;
        return;
    }
    std::stringstream ss;
    for (size_t i = 0; i < signature.size(); i++) {
        if(i % 55 == 0 && i != 0)
        {
            lines.push_back(ss.str());
            ss.str("");
        }
        ss << signature[i];
    }
    lines.push_back(ss.str());
    lines.push_back(END_LICENSE);
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

		if(Botan::check_passhash9("Coati "+ getVersion(), getHashLine()))
		{
	//        std::cout << "Hash from Coati "+ getVersion() + " confirmed" << std::endl;
		}

		Botan::secure_vector<Botan::byte> sig = Botan::base64_decode(getSignature());

		Botan::PK_Verifier verifier(*m_publicKey.get(), "EMSA4(SHA-256)");

		Botan::DataSource_Memory in(getMessage());
		Botan::byte buf[4096] = {0};
		while(size_t got = in.read(buf, sizeof(buf)))
		{
			verifier.update(buf, got);
		}

		const bool ok = verifier.check_signature(sig);
		return ok;
	}
	catch(...)
	{
		std::cout << "Invalid character in Licensekey" << std::endl;
	}
	return false;
}

bool License::isExpired() const
{
	return !getTimeLeft();
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
    if (!filename.empty()) {
        m_publicKeyFilename = filename;
    }
    if(boost::filesystem::exists(getPublicKeyFilename()))
    {
        Botan::RSA_PublicKey *rsaPublicKey = dynamic_cast<Botan::RSA_PublicKey *>(Botan::X509::load_key(getPublicKeyFilename()));
        if (!rsaPublicKey) {
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
    Botan::DataSource_Memory in(publicKey);
    Botan::RSA_PublicKey *rsaPublicKey = dynamic_cast<Botan::RSA_PublicKey *>(Botan::X509::load_key(in));
    if (!rsaPublicKey) {
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
    for(std::string line : lines)
    {
        license << line << std::endl;
    }
    return license.str();
}

std::string License::hashLocation(const std::string& location)
{
	return Botan::generate_passhash9(location, *(m_rng.get()));
}

bool License::checkLocation(const std::string& location, const std::string& hash)
{
    return Botan::check_passhash9(location, hash);
}

std::string License::getLicenseEncodedString(const std::string& applicationLocation) const
{
    Botan::AutoSeeded_RNG rng;
    std::vector<Botan::byte> file_contents;
    std::stringstream input(getLicenseString());

    //prepare the license string to work with the botan cryptobox
    while(input.good())
    {
        Botan::byte filebuf[4096] = { 0 };
        input.read((char*)filebuf, sizeof(filebuf));
        size_t got = input.gcount();

        file_contents.insert(file_contents.end(), filebuf, filebuf+got);
    }

    std::string ret = Botan::CryptoBox::encrypt(&file_contents[0], file_contents.size(),getEncodeKey(applicationLocation),rng);

    //remove Botan Cryptobox begin and end
    //should not be in the application settings
    ret = ret.substr(40,ret.length()-78);

    return ret;
}

bool License::loadFromEncodedString(const std::string& encodedLicense, const std::string& applicationLocation)
{
    try {
        //add cryptobox begin and end to loaded string
        std::string crypbobxInput = "-----BEGIN BOTAN CRYPTOBOX MESSAGE-----\n";
        crypbobxInput += encodedLicense;
        crypbobxInput += "-----END BOTAN CRYPTOBOX MESSAGE-----";

        //decrypt license
        loadFromString(Botan::CryptoBox::decrypt(crypbobxInput,getEncodeKey(applicationLocation)));
    }
    catch(...)
    {
        //loaded string from application settings is invalid
        return false;
    }
    return true;
}

std::string License::getEncodeKey(const std::string applicationLocation) const
{
    Botan::PBKDF *pbkdf = Botan::get_pbkdf("PBKDF2(SHA-256)");
    Botan::secure_vector<Botan::byte> salt = Botan::base64_decode("34zA54n60v4CxjY5n20k3J40c976n690", 32);
    Botan::AutoSeeded_RNG rng;
    return pbkdf->derive_key(32, applicationLocation, &salt[0], salt.size(), 10000).as_string();
}
