#ifndef VERSION_H
#define VERSION_H

#include <string>

class Version
{
public:
	static Version fromString(const std::string& versionString);

	static void setApplicationVersion(const Version& version);
	static const Version& getApplicationVersion();

	Version();

	bool operator<(const Version& other);
	bool isOlderStorageVersionThan(const Version& other);

	std::string toString() const;
	std::string toDisplayString() const;

private:
	static Version s_version;

	int m_majorNumber;
	int m_minorNumber;
	int m_refreshNumber;
	int m_commitNumber;

	std::string m_commitHash;
};

#endif // VERSION_H
