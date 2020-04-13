#ifndef SOURCE_GROUP_SETTINGS_CXX_CDB_H
#define SOURCE_GROUP_SETTINGS_CXX_CDB_H

#include "../SourceGroupSettingsWithComponents.h"
#include "../component/cxx/SourceGroupSettingsWithCxxCdbPath.h"
#include "../component/cxx/SourceGroupSettingsWithCxxPathsAndFlags.h"
#include "../component/cxx/SourceGroupSettingsWithCxxPchOptions.h"
#include "../component/SourceGroupSettingsWithExcludeFilters.h"
#include "../component/cxx/SourceGroupSettingsWithIndexedHeaderPaths.h"

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
