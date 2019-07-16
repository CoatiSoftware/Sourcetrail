#include "SourceGroupSettingsJavaGradle.h"

#include "ConfigManager.h"
#include "ProjectSettings.h"
#include "utilityFile.h"

SourceGroupSettingsJavaGradle::SourceGroupSettingsJavaGradle(const std::string& id, const ProjectSettings* projectSettings)
	: SourceGroupSettingsJava(id, SOURCE_GROUP_JAVA_GRADLE, projectSettings)
	, m_gradleProjectFilePath(FilePath())
	, m_shouldIndexGradleTests(false)
{
}

std::shared_ptr<SourceGroupSettings> SourceGroupSettingsJavaGradle::createCopy() const
{
	return std::make_shared<SourceGroupSettingsJavaGradle>(*this);
}

void SourceGroupSettingsJavaGradle::load(std::shared_ptr<const ConfigManager> config)
{
	SourceGroupSettingsJava::load(config);

	const std::string key = s_keyPrefix + getId();

	setGradleProjectFilePath(config->getValueOrDefault(key + "/gradle/project_file_path", FilePath(L"")));
	setShouldIndexGradleTests(config->getValueOrDefault(key + "/gradle/should_index_tests", false));
}

void SourceGroupSettingsJavaGradle::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettingsJava::save(config);

	const std::string key = s_keyPrefix + getId();

	config->setValue(key + "/gradle/project_file_path", getGradleProjectFilePath().wstr());
	config->setValue(key + "/gradle/should_index_tests", getShouldIndexGradleTests());
}

bool SourceGroupSettingsJavaGradle::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsJavaGradle> otherJavaGradle = std::dynamic_pointer_cast<SourceGroupSettingsJavaGradle>(other);

	return (
		otherJavaGradle &&
		SourceGroupSettingsJava::equals(other) &&
		m_gradleProjectFilePath == otherJavaGradle->m_gradleProjectFilePath &&
		m_shouldIndexGradleTests == otherJavaGradle->m_shouldIndexGradleTests
	);
}

FilePath SourceGroupSettingsJavaGradle::getGradleDependenciesDirectoryPath() const
{
	return getSourceGroupDependenciesDirectoryPath().concatenate(L"gradle");
}

FilePath SourceGroupSettingsJavaGradle::getGradleProjectFilePath() const
{
	return m_gradleProjectFilePath;
}

FilePath SourceGroupSettingsJavaGradle::getGradleProjectFilePathExpandedAndAbsolute() const
{
	return utility::getExpandedAndAbsolutePath(getGradleProjectFilePath(), getProjectSettings()->getProjectDirectoryPath());
}

void SourceGroupSettingsJavaGradle::setGradleProjectFilePath(const FilePath& path)
{
	m_gradleProjectFilePath = path;
}

bool SourceGroupSettingsJavaGradle::getShouldIndexGradleTests() const
{
	return m_shouldIndexGradleTests;
}

void SourceGroupSettingsJavaGradle::setShouldIndexGradleTests(bool value)
{
	m_shouldIndexGradleTests = value;
}
