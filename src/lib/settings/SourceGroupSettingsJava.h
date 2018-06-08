#ifndef SOURCE_GROUP_SETTINGS_JAVA_H
#define SOURCE_GROUP_SETTINGS_JAVA_H

#include <memory>
#include <vector>

#include "settings/SourceGroupSettings.h"
#include "settings/SourceGroupSettingsWithClasspath.h"
#include "settings/SourceGroupSettingsWithExcludeFilters.h"
#include "settings/SourceGroupSettingsWithJavaStandard.h"
#include "settings/SourceGroupSettingsWithSourceExtensions.h"
#include "settings/SourceGroupSettingsWithSourcePaths.h"

class SourceGroupSettingsJava
	: public SourceGroupSettings
	, public SourceGroupSettingsWithSourceExtensions
	, public SourceGroupSettingsWithSourcePaths
	, public SourceGroupSettingsWithExcludeFilters
	, public SourceGroupSettingsWithJavaStandard
	, public SourceGroupSettingsWithClasspath
{
public:
	SourceGroupSettingsJava(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings);

	void load(std::shared_ptr<const ConfigManager> config) override;
	void save(std::shared_ptr<ConfigManager> config) override;

	bool equals(std::shared_ptr<SourceGroupSettings> other) const override;

private:
	const ProjectSettings* getProjectSettings() const override;
	std::vector<std::wstring> getDefaultSourceExtensions() const override;
};

#endif // SOURCE_GROUP_SETTINGS_JAVA_H
