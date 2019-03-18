#include "License.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <istream>

#include <boost/date_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
// #include <botan_all.h>
#include <botan/auto_rng.h>
#include <botan/passhash9.h>

#include "Version.h"

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

std::string removeCaption(const std::string& line, const std::string& caption)
{
	if (line.substr(0, caption.length()) == caption)
	{
		return line.substr(caption.length());
	}
	return "";
}

}

License::License()
	: m_rng(std::make_unique<Botan::AutoSeeded_RNG>())
{
}

License::~License()
{
}

std::string License::getUser() const
{
	return m_user;
}

std::string License::getType() const
{
	return m_type;
}

size_t License::getNumberOfUsers() const
{
	return m_numberOfUsers;
}

std::string License::getLicenseInfo() const
{
	std::string info = m_user + "\n";

	info += m_type + "\n";

	// get info depending on license type
	if (m_numberOfUsers == 0)
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

	if (m_expirationVersion != LicenseConstants::VALID_UNLIMITED)
	{
		info += "valid up to version " + m_expirationVersion;
	}
	else if (m_expirationDate != LicenseConstants::VALID_UNLIMITED)
	{
		info += "valid until " + m_expirationDate;
	}
	else
	{
		info += "valid perpetually";
	}

	return info;
}

std::string License::getMessage(bool withNewlines) const
{
	if (m_user.empty() || m_type.empty() || m_expirationVersion.empty() || m_expirationDate.empty() || m_hashLine.empty())
	{
		return "";
	}

	const std::string separator = (withNewlines ? "\n" : "");
	std::string message = LicenseConstants::PRODUCT + separator;

	if (m_isOldFormat)
	{
		message += LicenseConstants::LICENSED_TO_OLD + m_user + separator;
		message += LicenseConstants::LICENSE_TYPE + m_type;

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

		if (m_expirationVersion != LicenseConstants::VALID_UNLIMITED)
		{
			message += LicenseConstants::VALID_UP_TO_OLD + m_expirationVersion + separator;
		}
		else if (m_expirationDate != LicenseConstants::VALID_UNLIMITED)
		{
			message += LicenseConstants::VALID_UNTIL_OLD + m_expirationDate + separator;
		}
	}
	else
	{
		std::string s;
		message += LicenseConstants::LICENSE_HOLDER + m_user + separator;
		message += LicenseConstants::LICENSE_TYPE + m_type + separator;

		if (m_numberOfUsers > 0)
		{
			message += LicenseConstants::LICENSED_USERS + std::to_string(m_numberOfUsers) + separator;
		}
		else
		{
			message += s + LicenseConstants::LICENSED_USERS + LicenseConstants::VALID_UNLIMITED + separator;
		}

		if (m_expirationVersion != LicenseConstants::VALID_UNLIMITED)
		{
			message += s + LicenseConstants::LICENSED_VERSION + LicenseConstants::VALID_UP_TO + m_expirationVersion + separator;
		}
		else
		{
			message += s + LicenseConstants::LICENSED_VERSION + LicenseConstants::VALID_UNLIMITED + separator;
		}

		if (m_expirationDate != LicenseConstants::VALID_UNLIMITED)
		{
			message += s + LicenseConstants::LICENSED_PERIOD + LicenseConstants::VALID_UNTIL + m_expirationDate + separator;
		}
		else
		{
			message += s + LicenseConstants::LICENSED_PERIOD + LicenseConstants::VALID_UNLIMITED + separator;
		}
	}

	message += LicenseConstants::SEPARATOR + separator;
	message += m_hashLine;

	return message;
}

void License::setMessage(
	const std::string& user,
	const std::string& type,
	size_t numberOfUsers,
	const std::string& expirationVersion,
	const std::string& expirationDate
){
	if (user.empty() || type.empty() || (numberOfUsers == 0 && expirationVersion.empty() && expirationDate.empty()))
	{
		return;
	}

	m_user = user;
	m_type = type;
	m_numberOfUsers = numberOfUsers;
	m_expirationVersion = expirationVersion.size() ? expirationVersion : LicenseConstants::VALID_UNLIMITED;
	m_expirationDate = expirationDate.size() ? expirationDate : LicenseConstants::VALID_UNLIMITED;
	m_hashLine = generateHash(user + type);
}

const std::string& License::getSignature() const
{
	return m_signature;
}

void License::setSignature(const std::string& signature)
{
	m_signature = signature;
}

int License::getTimeLeft() const
{
	const std::string dateText = "YYYY-MMM-DD";

	std::string dateString = m_expirationDate;
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

bool License::loadFromFile(const std::string& filename)
{
	std::ifstream sigfile(filename);
	return load(sigfile);
}

void License::print()
{
	std::cout << getLicenseString();
}

bool License::isEmpty() const
{
	if (m_user.empty() && m_type.empty() &&
		m_expirationVersion.empty() && m_expirationDate.empty() &&
		m_hashLine.empty() && m_signature.empty())
	{
		return true;
	}

	return false;
}

bool License::isComplete() const
{
	if (m_user.empty() || m_type.empty() ||
		m_expirationVersion.empty() || m_expirationDate.empty() ||
		m_hashLine.empty() || m_signature.empty())
	{
		return false;
	}

	return true;
}

bool License::isExpired() const
{
	if (m_expirationVersion != LicenseConstants::VALID_UNLIMITED)
	{
		Version version = Version::fromString(m_expirationVersion);
		return Version::getApplicationVersion().toShortVersion() > version;
	}
	else if (m_expirationDate != LicenseConstants::VALID_UNLIMITED)
	{
		return (getTimeLeft() == -1);
	}
	else if (m_expirationVersion == LicenseConstants::VALID_UNLIMITED &&
		m_expirationDate == LicenseConstants::VALID_UNLIMITED)
	{
		return false;
	}

	return true;
}

bool License::isTestLicense() const
{
	return m_type == LicenseConstants::TEST_LICENSE;
}

std::string License::getLicenseString() const
{
	if (m_user.empty() || m_type.empty() || m_expirationVersion.empty() || m_expirationDate.empty())
	{
		return "";
	}

	std::string signatureLines;
	std::stringstream ss;
	const int lineLength = 55;
	for (size_t i = 0; i < m_signature.size(); i++)
	{
		if (i % lineLength == 0 && i != 0)
		{
			signatureLines += ss.str() + "\n";
			ss.str("");
		}
		ss << m_signature[i];
	}
	signatureLines += ss.str();

	std::string license = "";
	license += LicenseConstants::BEGIN_LICENSE;
	license += "\n";
	license += getMessage(true) + "\n";
	license += signatureLines + "\n";
	license += LicenseConstants::END_LICENSE;
	license += "\n";

	return license;
}

std::string License::generateHash(const std::string& str) const
{
	if (m_rng == NULL || str.size() <= 0)
	{
		return "";
	}

	return Botan::generate_passhash9(str, *(m_rng.get()));
}

bool License::checkHash(const std::string& str, const std::string& hash)
{
	if (str.empty() || hash.empty())
	{
		return true;
	}

	return Botan::check_passhash9(str, hash);
}

bool License::load(std::istream& stream)
{
	std::string line;
	std::vector<std::string> lines;

	while (getline(stream, line, '\n'))
	{
		line = trim(line);

		if ((!lines.size() && line == LicenseConstants::BEGIN_LICENSE) || line == LicenseConstants::END_LICENSE)
		{
			continue;
		}

		if (line.size())
		{
			lines.push_back(line);
		}
	}

	if (lines.size() == 15)
	{
		m_isOldFormat = false;
		return loadFromLines(lines);
	}
	else if (lines.size() == 13)
	{
		m_isOldFormat = true;
		return loadFromLinesOld(lines);
	}

	return false;
}

bool License::loadFromLines(const std::vector<std::string>& lines)
{
	if (lines.size() != 15)
	{
		return false;
	}

	// sourcetrail line
	if (lines[0] != LicenseConstants::PRODUCT)
	{
		return false;
	}

	// user line
	m_user = removeCaption(lines[1], LicenseConstants::LICENSE_HOLDER);
	if (m_user.empty())
	{
		return false;
	}

	// type line
	m_type = removeCaption(lines[2], LicenseConstants::LICENSE_TYPE);
	if (m_type.empty())
	{
		return false;
	}

	// number of users
	std::string users = removeCaption(lines[3], LicenseConstants::LICENSED_USERS);
	if (users != LicenseConstants::VALID_UNLIMITED)
	{
		try
		{
			m_numberOfUsers = std::stoi(users);
		}
		catch (std::invalid_argument e)
		{
			m_numberOfUsers = 0;
			return false;
		}
	}
	else
	{
		m_numberOfUsers = 0;
	}

	// expiration version
	m_expirationVersion = removeCaption(lines[4], LicenseConstants::LICENSED_VERSION);
	if (m_expirationVersion != LicenseConstants::VALID_UNLIMITED)
	{
		m_expirationVersion = removeCaption(m_expirationVersion, LicenseConstants::VALID_UP_TO);
		if (!Version::fromString(m_expirationVersion).isValid())
		{
			return false;
		}
	}

	// expiration date
	m_expirationDate = removeCaption(lines[5], LicenseConstants::LICENSED_PERIOD);
	if (m_expirationDate != LicenseConstants::VALID_UNLIMITED)
	{
		m_expirationDate = removeCaption(m_expirationDate, LicenseConstants::VALID_UNTIL);

		if (m_expirationDate.size() != 11 || m_expirationDate[4] != '-' || m_expirationDate[8] != '-')
		{
			return false;
		}
	}

	// separator line
	if (lines[6] != LicenseConstants::SEPARATOR)
	{
		return false;
	}

	// hash line
	m_hashLine = lines[7];
	if (m_hashLine.length() != 55)
	{
		return false;
	}

	// signature
	m_signature = lines[8] + lines[9] + lines[10] + lines[11] + lines[12] + lines[13] + lines[14];
	if (m_signature.length() != 344)
	{
		return false;
	}

	return true;
}

bool License::loadFromLinesOld(const std::vector<std::string>& lines)
{
	if (lines.size() != 13)
	{
		return false;
	}

	// sourcetrail line
	if (lines[0] != LicenseConstants::PRODUCT)
	{
		return false;
	}

	// user line
	m_user = removeCaption(lines[1], LicenseConstants::LICENSED_TO_OLD);
	if (m_user.empty())
	{
		return false;
	}

	// type line
	{
		std::string line = removeCaption(lines[2], LicenseConstants::LICENSE_TYPE);
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

		if (m_type.empty())
		{
			return false;
		}
	}

	// expire line
	m_expirationVersion = removeCaption(lines[3], LicenseConstants::VALID_UP_TO_OLD);
	m_expirationDate = removeCaption(lines[3], LicenseConstants::VALID_UNTIL_OLD);
	if (m_expirationVersion.empty() && m_expirationDate.empty())
	{
		return false;
	}

	if (m_expirationVersion.empty())
	{
		m_expirationVersion = LicenseConstants::VALID_UNLIMITED;
	}

	if (m_expirationDate.empty())
	{
		m_expirationDate = LicenseConstants::VALID_UNLIMITED;
	}

	// separator line
	if (lines[4] != LicenseConstants::SEPARATOR)
	{
		return false;
	}

	// hash line
	m_hashLine = lines[5];
	if (m_hashLine.length() != 55)
	{
		return false;
	}

	// signature
	m_signature = lines[6] + lines[7] + lines[8] + lines[9] + lines[10] + lines[11] + lines[12];
	if (m_signature.length() != 344)
	{
		return false;
	}

	return true;
}
