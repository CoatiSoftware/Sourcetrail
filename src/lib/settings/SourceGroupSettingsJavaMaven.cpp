#include "settings/SourceGroupSettingsJavaMaven.h"

#include "settings/ProjectSettings.h"
#include "utility/ConfigManager.h"

SourceGroupSettingsJavaMaven::SourceGroupSettingsJavaMaven(const std::string& id, const ProjectSettings* projectSettings)
	: SourceGroupSettingsJava(id, SOURCE_GROUP_JAVA_MAVEN, projectSettings)
	, m_mavenProjectFilePath(FilePath())
	, m_mavenDependenciesDirectory(FilePath())
	, m_shouldIndexMavenTests(false)
{
}

std::shared_ptr<SourceGroupSettings> SourceGroupSettingsJavaMaven::createCopy() const
{
	return std::make_shared<SourceGroupSettingsJavaMaven>(*this);
}

void SourceGroupSettingsJavaMaven::load(std::shared_ptr<const ConfigManager> config)
{
	SourceGroupSettingsJava::load(config);

	const std::string key = s_keyPrefix + getId();

	setMavenProjectFilePath(config->getValueOrDefault(key + "/maven/project_file_path", FilePath(L"")));
	setMavenDependenciesDirectory(config->getValueOrDefault(key + "/maven/dependencies_directory", FilePath(L"")));
	setShouldIndexMavenTests(config->getValueOrDefault(key + "/maven/should_index_tests", false));
}

void SourceGroupSettingsJavaMaven::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettingsJava::save(config);

	const std::string key = s_keyPrefix + getId();

	config->setValue(key + "/maven/project_file_path", getMavenProjectFilePath().wstr());
	config->setValue(key + "/maven/dependencies_directory", getMavenDependenciesDirectory().wstr());
	config->setValue(key + "/maven/should_index_tests", getShouldIndexMavenTests());
}

bool SourceGroupSettingsJavaMaven::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsJavaMaven> otherJavaMaven = std::dynamic_pointer_cast<SourceGroupSettingsJavaMaven>(other);

	return (
		otherJavaMaven &&
		SourceGroupSettingsJava::equals(other) &&
		m_mavenProjectFilePath == otherJavaMaven->m_mavenProjectFilePath &&
		m_mavenDependenciesDirectory == otherJavaMaven->m_mavenDependenciesDirectory &&
		m_shouldIndexMavenTests == otherJavaMaven->m_shouldIndexMavenTests
	);
}

FilePath SourceGroupSettingsJavaMaven::getMavenProjectFilePath() const
{
	return m_mavenProjectFilePath;
}

FilePath SourceGroupSettingsJavaMaven::getMavenProjectFilePathExpandedAndAbsolute() const
{
	return m_projectSettings->makePathExpandedAndAbsolute(getMavenProjectFilePath());
}

void SourceGroupSettingsJavaMaven::setMavenProjectFilePath(const FilePath& path)
{
	m_mavenProjectFilePath = path;
}

FilePath SourceGroupSettingsJavaMaven::getMavenDependenciesDirectory() const
{
	return m_mavenDependenciesDirectory;
}

FilePath SourceGroupSettingsJavaMaven::getMavenDependenciesDirectoryExpandedAndAbsolute() const
{
	return m_projectSettings->makePathExpandedAndAbsolute(getMavenDependenciesDirectory());
}

void SourceGroupSettingsJavaMaven::setMavenDependenciesDirectory(const FilePath& path)
{
	m_mavenDependenciesDirectory = path;
}

bool SourceGroupSettingsJavaMaven::getShouldIndexMavenTests() const
{
	return m_shouldIndexMavenTests;
}

void SourceGroupSettingsJavaMaven::setShouldIndexMavenTests(bool value)
{
	m_shouldIndexMavenTests = value;
}

const ProjectSettings* SourceGroupSettingsJavaMaven::getProjectSettings() const
{
	return m_projectSettings;
}
