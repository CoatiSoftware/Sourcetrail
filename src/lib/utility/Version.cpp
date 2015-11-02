#include "utility/Version.h"

#include "utility/utilityString.h"
#include "utility/logging/logging.h"

Version Version::s_version;

Version Version::fromString(const std::string& versionString)
{
	Version version;
	std::vector<std::string> components = utility::splitToVector(versionString, '-');

	if (components.size() != 3)
	{
		LOG_ERROR("Version string is invalid: " + versionString);
		return version;
	}

	std::vector<std::string> numbers = utility::splitToVector(components[0], '.');
	if (numbers.size() > 0)
	{
		version.m_majorNumber = std::stoi(numbers[0]);
	}
	if (numbers.size() > 1)
	{
		version.m_minorNumber = std::stoi(numbers[1]);
	}
	if (numbers.size() > 2)
	{
		version.m_refreshNumber = std::stoi(numbers[2]);
	}

	version.m_commitNumber = std::stoi(components[1]);
	version.m_commitHash = components[2];

	return version;
}

void Version::setApplicationVersion(const Version& version)
{
	s_version = version;
}

const Version& Version::getApplicationVersion()
{
	return s_version;
}

Version::Version()
	: m_majorNumber(0)
	, m_minorNumber(0)
	, m_refreshNumber(0)
	, m_commitNumber(0)
{
}

bool Version::operator<(const Version& other)
{
	if (m_majorNumber != other.m_majorNumber)
	{
		return m_majorNumber < other.m_majorNumber;
	}
	else if (m_minorNumber != other.m_minorNumber)
	{
		return m_minorNumber < other.m_minorNumber;
	}
	else if (m_refreshNumber != other.m_refreshNumber)
	{
		return m_refreshNumber < other.m_refreshNumber;
	}
	else if (m_commitNumber != other.m_commitNumber)
	{
		return m_commitNumber < other.m_commitNumber;
	}
	else
	{
		return false;
	}
}

bool Version::isOlderStorageVersionThan(const Version& other)
{
	if (m_majorNumber != other.m_majorNumber)
	{
		return m_majorNumber < other.m_majorNumber;
	}
	else if (m_minorNumber != other.m_minorNumber)
	{
		return m_minorNumber < other.m_minorNumber;
	}
	else if (m_refreshNumber != other.m_refreshNumber)
	{
		return m_refreshNumber < other.m_refreshNumber;
	}
	else
	{
		return false;
	}
}

std::string Version::toString() const
{
	std::stringstream ss;
	ss << m_majorNumber << '.' << m_minorNumber << '.' << m_refreshNumber;
	ss << '-' << m_commitNumber << '-' << m_commitHash;
	return ss.str();
}

std::string Version::toDisplayString() const
{
	std::stringstream ss;
	ss << m_majorNumber << '.' << m_minorNumber << '.' << m_refreshNumber << '.' << m_commitNumber;
	return ss.str();
}
