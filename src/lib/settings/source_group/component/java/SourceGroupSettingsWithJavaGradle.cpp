#include "SourceGroupSettingsWithJavaGradle.h"

#include "ConfigManager.h"
#include "ProjectSettings.h"
#include "utilityFile.h"

FilePath SourceGroupSettingsWithJavaGradle::getGradleDependenciesDirectoryPath() const
{
	return getSourceGroupDependenciesDirectoryPath().concatenate(L"gradle");
}

FilePath SourceGroupSettingsWithJavaGradle::getGradleProjectFilePath() const
{
	return m_gradleProjectFilePath;
}

FilePath SourceGroupSettingsWithJavaGradle::getGradleProjectFilePathExpandedAndAbsolute() const
{
	return utility::getExpandedAndAbsolutePath(
		getGradleProjectFilePath(), getProjectSettings()->getProjectDirectoryPath());
}

void SourceGroupSettingsWithJavaGradle::setGradleProjectFilePath(const FilePath& path)
{
	m_gradleProjectFilePath = path;
}

bool SourceGroupSettingsWithJavaGradle::getShouldIndexGradleTests() const
{
	return m_shouldIndexGradleTests;
}

void SourceGroupSettingsWithJavaGradle::setShouldIndexGradleTests(bool value)
{
	m_shouldIndexGradleTests = value;
}

bool SourceGroupSettingsWithJavaGradle::equals(const SourceGroupSettingsBase* other) const
{
	const SourceGroupSettingsWithJavaGradle* otherPtr =
		dynamic_cast<const SourceGroupSettingsWithJavaGradle*>(other);

	return (
		otherPtr && m_gradleProjectFilePath == otherPtr->m_gradleProjectFilePath &&
		m_shouldIndexGradleTests == otherPtr->m_shouldIndexGradleTests);
}

void SourceGroupSettingsWithJavaGradle::load(const ConfigManager* config, const std::string& key)
{
	setGradleProjectFilePath(
		config->getValueOrDefault(key + "/gradle/project_file_path", FilePath(L"")));
	setShouldIndexGradleTests(config->getValueOrDefault(key + "/gradle/should_index_tests", false));
}

void SourceGroupSettingsWithJavaGradle::save(ConfigManager* config, const std::string& key)
{
	config->setValue(key + "/gradle/project_file_path", getGradleProjectFilePath().wstr());
	config->setValue(key + "/gradle/should_index_tests", getShouldIndexGradleTests());
}
