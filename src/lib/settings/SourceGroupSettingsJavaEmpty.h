#ifndef SOURCE_GROUP_SETTINGS_JAVA_EMPTY_H
#define SOURCE_GROUP_SETTINGS_JAVA_EMPTY_H

#include "settings/SourceGroupSettingsJava.h"
#include "settings/SourceGroupSettingsWithClasspath.h"
#include "settings/SourceGroupSettingsWithSourcePaths.h"

class SourceGroupSettingsJavaEmpty
	: public SourceGroupSettingsJava
	, public SourceGroupSettingsWithSourcePaths
	, public SourceGroupSettingsWithClasspath
{
public:
	SourceGroupSettingsJavaEmpty(const std::string& id, const ProjectSettings* projectSettings);

	void load(std::shared_ptr<const ConfigManager> config) override;
	void save(std::shared_ptr<ConfigManager> config) override;

	bool equals(std::shared_ptr<SourceGroupSettings> other) const override;

private:
	const ProjectSettings* getProjectSettings() const override;
};

#endif // SOURCE_GROUP_SETTINGS_JAVA_EMPTY_H
