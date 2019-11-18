#ifndef SOURCE_GROUP_SETTINGS_WITH_JAVA_MAVEN_H
#define SOURCE_GROUP_SETTINGS_WITH_JAVA_MAVEN_H

#include "FilePath.h"
#include "SourceGroupSettingsComponent.h"

class SourceGroupSettingsWithJavaMaven: public SourceGroupSettingsComponent
{
public:
	virtual ~SourceGroupSettingsWithJavaMaven() = default;

	FilePath getMavenDependenciesDirectoryPath() const;

	FilePath getMavenProjectFilePath() const;
	FilePath getMavenProjectFilePathExpandedAndAbsolute() const;
	void setMavenProjectFilePath(const FilePath& path);

	bool getShouldIndexMavenTests() const;
	void setShouldIndexMavenTests(bool value);

protected:
	bool equals(const SourceGroupSettingsBase* other) const override;

	void load(const ConfigManager* config, const std::string& key) override;
	void save(ConfigManager* config, const std::string& key) override;

private:
	FilePath m_mavenProjectFilePath;
	bool m_shouldIndexMavenTests = false;
};

#endif	  // SOURCE_GROUP_SETTINGS_WITH_JAVA_MAVEN_H
