#ifndef SOURCE_GROUP_SETTINGS_JAVA_H
#define SOURCE_GROUP_SETTINGS_JAVA_H

#include <memory>
#include <vector>

#include "SourceGroupSettings.h"
#include "SourceGroupSettingsWithExcludeFilters.h"
#include "SourceGroupSettingsWithJavaStandard.h"
#include "SourceGroupSettingsWithSourceExtensions.h"

class SourceGroupSettingsJava
	: public SourceGroupSettings
	, public SourceGroupSettingsWithSourceExtensions
	, public SourceGroupSettingsWithExcludeFilters
	, public SourceGroupSettingsWithJavaStandard
{
public:
	SourceGroupSettingsJava(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings);

	void load(std::shared_ptr<const ConfigManager> config) override;
	void save(std::shared_ptr<ConfigManager> config) override;

	bool equals(std::shared_ptr<SourceGroupSettings> other) const override;

private:
	std::vector<std::wstring> getDefaultSourceExtensions() const override;
};

#endif // SOURCE_GROUP_SETTINGS_JAVA_H
