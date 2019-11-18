#ifndef SOURCE_GROUP_SETTINGS_CXX_CDB_H
#define SOURCE_GROUP_SETTINGS_CXX_CDB_H

#include "SourceGroupSettingsWithComponents.h"
#include "SourceGroupSettingsWithCxxCdbPath.h"
#include "SourceGroupSettingsWithCxxPathsAndFlags.h"
#include "SourceGroupSettingsWithCxxPchOptions.h"
#include "SourceGroupSettingsWithExcludeFilters.h"
#include "SourceGroupSettingsWithIndexedHeaderPaths.h"

class SourceGroupSettingsCxxCdb
	: public SourceGroupSettingsWithComponents<
		  SourceGroupSettingsWithCxxCdbPath,
		  SourceGroupSettingsWithCxxPathsAndFlags,
		  SourceGroupSettingsWithCxxPchOptions,
		  SourceGroupSettingsWithExcludeFilters,
		  SourceGroupSettingsWithIndexedHeaderPaths>
{
public:
	SourceGroupSettingsCxxCdb(const std::string& id, const ProjectSettings* projectSettings)
		: SourceGroupSettingsWithComponents(SOURCE_GROUP_CXX_CDB, id, projectSettings)
	{
	}

	std::shared_ptr<SourceGroupSettings> createCopy() const override
	{
		return std::make_shared<SourceGroupSettingsCxxCdb>(*this);
	}
};

#endif	  // SOURCE_GROUP_SETTINGS_CXX_CDB_H
