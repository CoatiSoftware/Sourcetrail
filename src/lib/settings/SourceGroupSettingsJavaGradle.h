#ifndef SOURCE_GROUP_SETTINGS_JAVA_GRADLE_H
#define SOURCE_GROUP_SETTINGS_JAVA_GRADLE_H

#include "settings/SourceGroupSettingsJava.h"
#include "utility/file/FilePath.h"

class SourceGroupSettingsJavaGradle
	: public SourceGroupSettingsJava
{
public:
	SourceGroupSettingsJavaGradle(const std::string& id, const ProjectSettings* projectSettings);

	std::shared_ptr<SourceGroupSettings> createCopy() const override;

	void load(std::shared_ptr<const ConfigManager> config) override;
	void save(std::shared_ptr<ConfigManager> config) override;

	bool equals(std::shared_ptr<SourceGroupSettings> other) const override;

	FilePath getGradleProjectFilePath() const;
	FilePath getGradleProjectFilePathExpandedAndAbsolute() const;
	void setGradleProjectFilePath(const FilePath& path);

	FilePath getGradleDependenciesDirectory() const;
	FilePath getGradleDependenciesDirectoryExpandedAndAbsolute() const;
	void setGradleDependenciesDirectory(const FilePath& path);

	bool getShouldIndexGradleTests() const;
	void setShouldIndexGradleTests(bool value);

private:
	const ProjectSettings* getProjectSettings() const override;

	FilePath m_gradleProjectFilePath;
	FilePath m_gradleDependenciesDirectory;
	bool m_shouldIndexGradleTests;
};

#endif // SOURCE_GROUP_SETTINGS_JAVA_GRADLE_H
