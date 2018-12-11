#ifndef SOURCE_GROUP_SETTINGS_CPP_EMPTY_H
#define SOURCE_GROUP_SETTINGS_CPP_EMPTY_H

#include "SourceGroupSettingsCxx.h"
#include "SourceGroupSettingsWithCppStandard.h"
#include "SourceGroupSettingsWithCxxCrossCompilationOptions.h"
#include "SourceGroupSettingsWithExcludeFilters.h"
#include "SourceGroupSettingsWithSourceExtensions.h"
#include "SourceGroupSettingsWithSourcePaths.h"

class SourceGroupSettingsCppEmpty
	: public SourceGroupSettingsCxx
	, public SourceGroupSettingsWithCppStandard
	, public SourceGroupSettingsWithCxxCrossCompilationOptions
	, public SourceGroupSettingsWithExcludeFilters
	, public SourceGroupSettingsWithSourceExtensions
	, public SourceGroupSettingsWithSourcePaths
{
public:
	SourceGroupSettingsCppEmpty(const std::string& id, const ProjectSettings* projectSettings);

	std::shared_ptr<SourceGroupSettings> createCopy() const override;

	void load(std::shared_ptr<const ConfigManager> config) override;
	void save(std::shared_ptr<ConfigManager> config) override;

	bool equals(std::shared_ptr<SourceGroupSettings> other) const override;

private:
	std::vector<std::wstring> getDefaultSourceExtensions() const override;
};

#endif // SOURCE_GROUP_SETTINGS_CPP_EMPTY_H
