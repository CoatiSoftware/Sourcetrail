#include "SourceGroupSettingsJavaMaven.h"

#include "ConfigManager.h"
#include "ProjectSettings.h"
#include "utilityFile.h"

SourceGroupSettingsJavaMaven::SourceGroupSettingsJavaMaven(const std::string& id, const ProjectSettings* projectSettings)
	: SourceGroupSettingsJava(id, SOURCE_GROUP_JAVA_MAVEN, projectSettings)
	, m_mavenProjectFilePath(FilePath())
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
	setShouldIndexMavenTests(config->getValueOrDefault(key + "/maven/should_index_tests", false));
}

void SourceGroupSettingsJavaMaven::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettingsJava::save(config);

	const std::string key = s_keyPrefix + getId();

	config->setValue(key + "/maven/project_file_path", getMavenProjectFilePath().wstr());
	config->setValue(key + "/maven/should_index_tests", getShouldIndexMavenTests());
}

bool SourceGroupSettingsJavaMaven::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsJavaMaven> otherJavaMaven = std::dynamic_pointer_cast<SourceGroupSettingsJavaMaven>(other);

	return (
		otherJavaMaven &&
		SourceGroupSettingsJava::equals(other) &&
		m_mavenProjectFilePath == otherJavaMaven->m_mavenProjectFilePath &&
		m_shouldIndexMavenTests == otherJavaMaven->m_shouldIndexMavenTests
	);
}

FilePath SourceGroupSettingsJavaMaven::getMavenDependenciesDirectoryPath() const
{
	return getSourceGroupDependenciesDirectoryPath().concatenate(L"maven");
}

FilePath SourceGroupSettingsJavaMaven::getMavenProjectFilePath() const
{
	return m_mavenProjectFilePath;
}

FilePath SourceGroupSettingsJavaMaven::getMavenProjectFilePathExpandedAndAbsolute() const
{
	return utility::getExpandedAndAbsolutePath(getMavenProjectFilePath(), getProjectSettings()->getProjectDirectoryPath());
}

void SourceGroupSettingsJavaMaven::setMavenProjectFilePath(const FilePath& path)
{
	m_mavenProjectFilePath = path;
}

bool SourceGroupSettingsJavaMaven::getShouldIndexMavenTests() const
{
	return m_shouldIndexMavenTests;
}

void SourceGroupSettingsJavaMaven::setShouldIndexMavenTests(bool value)
{
	m_shouldIndexMavenTests = value;
}
