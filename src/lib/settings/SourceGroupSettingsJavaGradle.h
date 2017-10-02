#ifndef SOURCE_GROUP_SETTINGS_JAVA_GRADLE_H
#define SOURCE_GROUP_SETTINGS_JAVA_GRADLE_H

#include "settings/SourceGroupSettingsJava.h"

class SourceGroupSettingsJavaGradle
	: public SourceGroupSettingsJava
{
public:
	SourceGroupSettingsJavaGradle(const std::string& id, const ProjectSettings* projectSettings);
	virtual ~SourceGroupSettingsJavaGradle();

	virtual void load(std::shared_ptr<const ConfigManager> config);
	virtual void save(std::shared_ptr<ConfigManager> config);

	virtual bool equals(std::shared_ptr<SourceGroupSettings> other) const;

	FilePath getGradleProjectFilePath() const;
	FilePath getGradleProjectFilePathExpandedAndAbsolute() const;
	void setGradleProjectFilePath(const FilePath& path);

	FilePath getGradleDependenciesDirectory() const;
	FilePath getGradleDependenciesDirectoryExpandedAndAbsolute() const;
	void setGradleDependenciesDirectory(const FilePath& path);

	bool getShouldIndexGradleTests() const;
	void setShouldIndexGradleTests(bool value);

private:
	FilePath m_gradleProjectFilePath;
	FilePath m_gradleDependenciesDirectory;
	bool m_shouldIndexGradleTests;
};

#endif // SOURCE_GROUP_SETTINGS_JAVA_GRADLE_H
