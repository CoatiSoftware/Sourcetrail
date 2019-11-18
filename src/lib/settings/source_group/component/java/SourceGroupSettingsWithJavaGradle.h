#ifndef SOURCE_GROUP_SETTINGS_WITH_JAVA_GRADLE_H
#define SOURCE_GROUP_SETTINGS_WITH_JAVA_GRADLE_H

#include "FilePath.h"
#include "SourceGroupSettingsComponent.h"

class SourceGroupSettingsWithJavaGradle: public SourceGroupSettingsComponent
{
public:
	virtual ~SourceGroupSettingsWithJavaGradle() = default;

	FilePath getGradleDependenciesDirectoryPath() const;

	FilePath getGradleProjectFilePath() const;
	FilePath getGradleProjectFilePathExpandedAndAbsolute() const;
	void setGradleProjectFilePath(const FilePath& path);

	bool getShouldIndexGradleTests() const;
	void setShouldIndexGradleTests(bool value);

protected:
	bool equals(const SourceGroupSettingsBase* other) const override;

	void load(const ConfigManager* config, const std::string& key) override;
	void save(ConfigManager* config, const std::string& key) override;

private:
	FilePath m_gradleProjectFilePath;
	bool m_shouldIndexGradleTests = false;
};

#endif	  // SOURCE_GROUP_SETTINGS_WITH_JAVA_GRADLE_H
