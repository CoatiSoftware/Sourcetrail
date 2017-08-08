#ifndef VERSION_H
#define VERSION_H

#include <string>

class Version
{
public:
	static Version fromString(const std::string& versionString);
	static Version fromShortString(const std::string& versionString);

	static void setApplicationVersion(const Version& version);
	static const Version& getApplicationVersion();

	Version(int year = 0, int minor = 0, int commit = 0, const std::string& hash = "");

	bool isEmpty() const;

	std::string toString() const;
	std::string toShortString() const;
	std::string toDisplayString() const;

	bool operator<(const Version& other) const;
	bool operator>(const Version& other) const;
	Version& operator+=(const int& number);
	bool isValid();

private:
	static Version s_version;

	int m_year;
	int m_minorNumber;
	int m_commitNumber;

	std::string m_commitHash;
};

#endif // VERSION_H
