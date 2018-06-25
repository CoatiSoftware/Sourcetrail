#include "settings/SourceGroupSettingsCxxCdbVs.h"

SourceGroupSettingsCxxCdbVs::SourceGroupSettingsCxxCdbVs(const std::string& id, const ProjectSettings* projectSettings)
	: SourceGroupSettingsCxxCdb(id, projectSettings)
{
}

std::shared_ptr<SourceGroupSettings> SourceGroupSettingsCxxCdbVs::createCopy() const
{
	return std::make_shared<SourceGroupSettingsCxxCdbVs>(*this);
}
