#ifndef SOURCE_GROUP_SETTINGS_CXX_CDB_VS_H
#define SOURCE_GROUP_SETTINGS_CXX_CDB_VS_H

#include "settings/SourceGroupSettingsCxxCdb.h"
#include "utility/file/FilePath.h"

class SourceGroupSettingsCxxCdbVs
	: public SourceGroupSettingsCxxCdb
{
public:
	SourceGroupSettingsCxxCdbVs(const std::string& id, const ProjectSettings* projectSettings);

	std::shared_ptr<SourceGroupSettings> createCopy() const override;
};

#endif // SOURCE_GROUP_SETTINGS_CXX_CDB_VS_H
