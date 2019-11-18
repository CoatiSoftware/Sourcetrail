#ifndef SOURCE_GROUP_SETTINGS_C_EMPTY_H
#define SOURCE_GROUP_SETTINGS_C_EMPTY_H

#include "SourceGroupSettingsWithCStandard.h"
#include "SourceGroupSettingsWithComponents.h"
#include "SourceGroupSettingsWithCxxCrossCompilationOptions.h"
#include "SourceGroupSettingsWithCxxPathsAndFlags.h"
#include "SourceGroupSettingsWithCxxPchOptions.h"
#include "SourceGroupSettingsWithExcludeFilters.h"
#include "SourceGroupSettingsWithSourceExtensionsC.h"
#include "SourceGroupSettingsWithSourcePaths.h"

class SourceGroupSettingsCEmpty
	: public SourceGroupSettingsWithComponents<
		  SourceGroupSettingsWithCStandard,
		  SourceGroupSettingsWithCxxCrossCompilationOptions,
		  SourceGroupSettingsWithCxxPathsAndFlags,
		  SourceGroupSettingsWithCxxPchOptions,
		  SourceGroupSettingsWithExcludeFilters,
		  SourceGroupSettingsWithSourceExtensionsC,
		  SourceGroupSettingsWithSourcePaths>
{
public:
	SourceGroupSettingsCEmpty(const std::string& id, const ProjectSettings* projectSettings)
		: SourceGroupSettingsWithComponents(SOURCE_GROUP_C_EMPTY, id, projectSettings)
	{
	}

	std::shared_ptr<SourceGroupSettings> createCopy() const override
	{
		return std::make_shared<SourceGroupSettingsCEmpty>(*this);
	}
};

#endif	  // SOURCE_GROUP_SETTINGS_C_EMPTY_H
