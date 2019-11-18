#ifndef SOURCE_GROUP_SETTINGS_CPP_EMPTY_H
#define SOURCE_GROUP_SETTINGS_CPP_EMPTY_H

#include "language_packages.h"

#if BUILD_CXX_LANGUAGE_PACKAGE

#	include "SourceGroupSettingsWithComponents.h"
#	include "SourceGroupSettingsWithCppStandard.h"
#	include "SourceGroupSettingsWithCxxCrossCompilationOptions.h"
#	include "SourceGroupSettingsWithCxxPathsAndFlags.h"
#	include "SourceGroupSettingsWithCxxPchOptions.h"
#	include "SourceGroupSettingsWithExcludeFilters.h"
#	include "SourceGroupSettingsWithSourceExtensionsCpp.h"
#	include "SourceGroupSettingsWithSourcePaths.h"

class SourceGroupSettingsCppEmpty
	: public SourceGroupSettingsWithComponents<
		  SourceGroupSettingsWithCppStandard,
		  SourceGroupSettingsWithCxxCrossCompilationOptions,
		  SourceGroupSettingsWithCxxPathsAndFlags,
		  SourceGroupSettingsWithCxxPchOptions,
		  SourceGroupSettingsWithExcludeFilters,
		  SourceGroupSettingsWithSourceExtensionsCpp,
		  SourceGroupSettingsWithSourcePaths>
{
public:
	SourceGroupSettingsCppEmpty(const std::string& id, const ProjectSettings* projectSettings)
		: SourceGroupSettingsWithComponents(SOURCE_GROUP_CPP_EMPTY, id, projectSettings)
	{
	}

	std::shared_ptr<SourceGroupSettings> createCopy() const override
	{
		return std::make_shared<SourceGroupSettingsCppEmpty>(*this);
	}
};

#endif	  // BUILD_CXX_LANGUAGE_PACKAGE

#endif	  // SOURCE_GROUP_SETTINGS_CPP_EMPTY_H
