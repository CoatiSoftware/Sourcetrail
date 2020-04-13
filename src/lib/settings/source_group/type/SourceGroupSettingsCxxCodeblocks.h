#ifndef SOURCE_GROUP_SETTINGS_CXX_CODEBLOCKS_H
#define SOURCE_GROUP_SETTINGS_CXX_CODEBLOCKS_H

#include "../component/cxx/SourceGroupSettingsWithCStandard.h"
#include "../SourceGroupSettingsWithComponents.h"
#include "../component/cxx/SourceGroupSettingsWithCppStandard.h"
#include "../component/cxx/SourceGroupSettingsWithCxxCodeblocksPath.h"
#include "../component/cxx/SourceGroupSettingsWithCxxPathsAndFlags.h"
#include "../component/SourceGroupSettingsWithExcludeFilters.h"
#include "../component/cxx/SourceGroupSettingsWithIndexedHeaderPaths.h"
#include "../component/cxx/SourceGroupSettingsWithSourceExtensionsCxx.h"

class SourceGroupSettingsCxxCodeblocks
	: public SourceGroupSettingsWithComponents<
		  SourceGroupSettingsWithCppStandard,
		  SourceGroupSettingsWithCStandard,
		  SourceGroupSettingsWithCxxCodeblocksPath,
		  SourceGroupSettingsWithCxxPathsAndFlags,
		  SourceGroupSettingsWithExcludeFilters,
		  SourceGroupSettingsWithIndexedHeaderPaths,
		  SourceGroupSettingsWithSourceExtensionsCxx>
{
public:
	SourceGroupSettingsCxxCodeblocks(const std::string& id, const ProjectSettings* projectSettings)
		: SourceGroupSettingsWithComponents(SOURCE_GROUP_CXX_CODEBLOCKS, id, projectSettings)
	{
	}

	std::shared_ptr<SourceGroupSettings> createCopy() const override
	{
		return std::make_shared<SourceGroupSettingsCxxCodeblocks>(*this);
	}
};

#endif	  // SOURCE_GROUP_SETTINGS_CXX_CODEBLOCKS_H
