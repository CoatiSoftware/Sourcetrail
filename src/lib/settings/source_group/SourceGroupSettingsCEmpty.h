#ifndef SOURCE_GROUP_SETTINGS_C_EMPTY_H
#define SOURCE_GROUP_SETTINGS_C_EMPTY_H

#include "SourceGroupSettingsCxx.h"
#include "SourceGroupSettingsWithCStandard.h"
#include "SourceGroupSettingsWithCxxCrossCompilationOptions.h"
#include "SourceGroupSettingsWithExcludeFilters.h"
#include "SourceGroupSettingsWithCxxPchOptions.h"
#include "SourceGroupSettingsWithSourceExtensions.h"
#include "SourceGroupSettingsWithSourcePaths.h"

class SourceGroupSettingsCEmpty
	: public SourceGroupSettingsCxx
	, public SourceGroupSettingsWithCStandard
	, public SourceGroupSettingsWithCxxCrossCompilationOptions
	, public SourceGroupSettingsWithExcludeFilters
	, public SourceGroupSettingsWithCxxPchOptions
	, public SourceGroupSettingsWithSourceExtensions
	, public SourceGroupSettingsWithSourcePaths
{
public:
	SourceGroupSettingsCEmpty(const std::string& id, const ProjectSettings* projectSettings);

	std::shared_ptr<SourceGroupSettings> createCopy() const override;

	void load(std::shared_ptr<const ConfigManager> config) override;
	void save(std::shared_ptr<ConfigManager> config) override;

	bool equals(std::shared_ptr<SourceGroupSettings> other) const override;

private:
	std::vector<std::wstring> getDefaultSourceExtensions() const override;
};

#endif // SOURCE_GROUP_SETTINGS_C_EMPTY_H
