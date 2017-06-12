#include "utility/Version.h"

#include <sstream>

// since there are 4 version per year it is on digit
const int MINOR_VERSION_SHIFT = 10;

Version Version::s_version;

#include <iostream>
Version Version::fromShortString(const std::string& versionString)
{
    Version version;
    if (versionString.length() != 6 || versionString[4] != '.')
    {
        return version;
    }

    try
    {
        version.m_year = std::stoi(versionString.substr(0,4));
        version.m_minorNumber = std::stoi(versionString.substr(5));
    }
    catch (std::invalid_argument e)
    {
        return Version();
    }

    return version;
}

bool Version::operator<(const Version& other) const
{
    return (m_year * MINOR_VERSION_SHIFT + m_minorNumber) < (other.m_year * MINOR_VERSION_SHIFT + other.m_minorNumber);
}

bool Version::operator>(const Version& other) const
{
    return (m_year * MINOR_VERSION_SHIFT + m_minorNumber) > (other.m_year * MINOR_VERSION_SHIFT + other.m_minorNumber);
}

Version& Version::operator +=(const int& number)
{
    int minor = this->m_minorNumber - 1 + number;
    this->m_year += minor/4;
    this->m_minorNumber = (minor%4) + 1;
    return *this;
}

bool Version::isValid()
{
    if (m_minorNumber < 5 && m_minorNumber > 0
            && m_year > 2016)
    {
        return true;
    }
    return false;
}

void Version::setApplicationVersion(const Version& version)
{
	s_version = version;
}

const Version& Version::getApplicationVersion()
{
	return s_version;
}

Version::Version(int year, int minor, int commit, const std::string& hash)
    : m_year(year)
    , m_minorNumber(minor)
    , m_commitNumber(commit)
    , m_commitHash(hash)
{
}

bool Version::isEmpty() const
{
    return m_year == 0 && m_minorNumber == 0 && m_commitNumber == 0;
}

std::string Version::toShortString() const
{
    std::stringstream ss;
    ss << m_year << '.' << m_minorNumber;
    return ss.str();
}

std::string Version::toString() const
{
	std::stringstream ss;
    ss << m_year << '.' << m_minorNumber;
    ss << '-' << m_commitNumber << '-' << m_commitHash;
	return ss.str();
}

std::string Version::toDisplayString() const
{
	std::stringstream ss;
    ss << m_year << '.' << m_minorNumber;
	ss << '.' <<  m_commitNumber;
	return ss.str();
}
