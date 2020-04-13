#ifndef SOURCE_GROUP_SETTINGS_C_EMPTY_H
#define SOURCE_GROUP_SETTINGS_C_EMPTY_H

#include "../component/cxx/SourceGroupSettingsWithCStandard.h"
#include "../SourceGroupSettingsWithComponents.h"
#include "../component/cxx/SourceGroupSettingsWithCxxCrossCompilationOptions.h"
#include "../component/cxx/SourceGroupSettingsWithCxxPathsAndFlags.h"
#include "../component/cxx/SourceGroupSettingsWithCxxPchOptions.h"
#include "../component/SourceGroupSettingsWithExcludeFilters.h"
#include "../component/cxx/SourceGroupSettingsWithSourceExtensionsC.h"
#include "../component/SourceGroupSettingsWithSourcePaths.h"

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
