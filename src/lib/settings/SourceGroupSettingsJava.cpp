#include "settings/SourceGroupSettingsJava.h"

#include "utility/utility.h"

SourceGroupSettingsJava::SourceGroupSettingsJava(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings)
	: SourceGroupSettings(id, type, projectSettings)
	, m_classpaths(std::vector<FilePath>())
	, m_mavenProjectFilePath(FilePath())
	, m_mavenDependenciesDirectory(FilePath())
	, m_shouldIndexMavenTests(false)
{
}

SourceGroupSettingsJava::~SourceGroupSettingsJava()
{
}

bool SourceGroupSettingsJava::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsJava> otherJava = std::dynamic_pointer_cast<SourceGroupSettingsJava>(other);

	return (
		otherJava &&
		SourceGroupSettings::equals(other) &&
		utility::isPermutation(m_classpaths, otherJava->m_classpaths) &&
		m_mavenProjectFilePath == otherJava->m_mavenProjectFilePath &&
		m_mavenDependenciesDirectory == otherJava->m_mavenDependenciesDirectory &&
		m_shouldIndexMavenTests == otherJava->m_shouldIndexMavenTests
	);
}

std::vector<std::string> SourceGroupSettingsJava::getAvailableLanguageStandards() const
{
	return std::vector<std::string>(1, "8");
}

std::vector<FilePath> SourceGroupSettingsJava::getClasspaths() const
{
	return m_classpaths;
}

std::vector<FilePath> SourceGroupSettingsJava::getClasspathsExpandedAndAbsolute() const
{
	return m_projectSettings->makePathsExpandedAndAbsolute(getClasspaths());
}

void SourceGroupSettingsJava::setClasspaths(const std::vector<FilePath>& classpaths)
{
	m_classpaths = classpaths;
}

FilePath SourceGroupSettingsJava::getMavenProjectFilePath() const
{
	return m_mavenProjectFilePath;
}

FilePath SourceGroupSettingsJava::getMavenProjectFilePathExpandedAndAbsolute() const
{
	return m_projectSettings->makePathExpandedAndAbsolute(getMavenProjectFilePath());
}

void SourceGroupSettingsJava::setMavenProjectFilePath(const FilePath& path)
{
	m_mavenProjectFilePath = path;
}

FilePath SourceGroupSettingsJava::getMavenDependenciesDirectory() const
{
	return m_mavenDependenciesDirectory;
}

FilePath SourceGroupSettingsJava::getMavenDependenciesDirectoryExpandedAndAbsolute() const
{
	return m_projectSettings->makePathExpandedAndAbsolute(getMavenDependenciesDirectory());
}

void SourceGroupSettingsJava::setMavenDependenciesDirectory(const FilePath& path)
{
	m_mavenDependenciesDirectory = path;
}

bool SourceGroupSettingsJava::getShouldIndexMavenTests() const
{
	return m_shouldIndexMavenTests;
}

void SourceGroupSettingsJava::setShouldIndexMavenTests(bool value)
{
	m_shouldIndexMavenTests = value;
}

std::vector<std::string> SourceGroupSettingsJava::getDefaultSourceExtensions() const
{
	return std::vector<std::string>(1, ".java");
}

std::string SourceGroupSettingsJava::getDefaultStandard() const
{
	return "8";
}
