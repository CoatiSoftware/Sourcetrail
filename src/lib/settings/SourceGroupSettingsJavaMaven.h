#ifndef SOURCE_GROUP_SETTINGS_JAVA_MAVEN_H
#define SOURCE_GROUP_SETTINGS_JAVA_MAVEN_H

#include "SourceGroupSettingsJava.h"
#include "FilePath.h"

class SourceGroupSettingsJavaMaven
	: public SourceGroupSettingsJava
{
public:
	SourceGroupSettingsJavaMaven(const std::string& id, const ProjectSettings* projectSettings);

	std::shared_ptr<SourceGroupSettings> createCopy() const override;

	void load(std::shared_ptr<const ConfigManager> config) override;
	void save(std::shared_ptr<ConfigManager> config) override;

	bool equals(std::shared_ptr<SourceGroupSettings> other) const override;

	FilePath getMavenProjectFilePath() const;
	FilePath getMavenProjectFilePathExpandedAndAbsolute() const;
	void setMavenProjectFilePath(const FilePath& path);

	FilePath getMavenDependenciesDirectory() const;
	FilePath getMavenDependenciesDirectoryExpandedAndAbsolute() const;
	void setMavenDependenciesDirectory(const FilePath& path);

	bool getShouldIndexMavenTests() const;
	void setShouldIndexMavenTests(bool value);

private:
	FilePath m_mavenProjectFilePath;
	FilePath m_mavenDependenciesDirectory;
	bool m_shouldIndexMavenTests;
};

#endif // SOURCE_GROUP_SETTINGS_JAVA_MAVEN_H
