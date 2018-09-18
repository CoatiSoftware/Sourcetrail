#ifndef SOURCE_GROUP_SETTINGS_JAVA_SONARGRAPH_H
#define SOURCE_GROUP_SETTINGS_JAVA_SONARGRAPH_H

#include "SourceGroupSettings.h"
#include "SourceGroupSettingsWithClasspath.h"
#include "SourceGroupSettingsWithJavaStandard.h"
#include "SourceGroupSettingsWithSonargraphProjectPath.h"

class SourceGroupSettingsJavaSonargraph
	: public SourceGroupSettings
	, public SourceGroupSettingsWithClasspath
	, public SourceGroupSettingsWithJavaStandard
	, public SourceGroupSettingsWithSonargraphProjectPath
{
public:
	SourceGroupSettingsJavaSonargraph(const std::string& id, const ProjectSettings* projectSettings);

	std::shared_ptr<SourceGroupSettings> createCopy() const override;

	void load(std::shared_ptr<const ConfigManager> config) override;
	void save(std::shared_ptr<ConfigManager> config) override;

	bool equals(std::shared_ptr<SourceGroupSettings> other) const override;

private:
	const ProjectSettings* getProjectSettings() const override;
};

#endif // SOURCE_GROUP_SETTINGS_JAVA_SONARGRAPH_H
