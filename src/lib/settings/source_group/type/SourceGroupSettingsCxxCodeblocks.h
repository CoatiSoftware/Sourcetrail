#ifndef SOURCE_GROUP_SETTINGS_CXX_CODEBLOCKS_H
#define SOURCE_GROUP_SETTINGS_CXX_CODEBLOCKS_H

#include "SourceGroupSettingsWithComponents.h"
#include "SourceGroupSettingsWithCppStandard.h"
#include "SourceGroupSettingsWithCStandard.h"
#include "SourceGroupSettingsWithCxxCodeblocksPath.h"
#include "SourceGroupSettingsWithCxxPathsAndFlags.h"
#include "SourceGroupSettingsWithExcludeFilters.h"
#include "SourceGroupSettingsWithIndexedHeaderPaths.h"
#include "SourceGroupSettingsWithSourceExtensionsCxx.h"

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

#endif // SOURCE_GROUP_SETTINGS_CXX_CODEBLOCKS_H
