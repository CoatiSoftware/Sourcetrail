#include "project/SourceGroupFactoryModuleCxx.h"

#include "project/SourceGroupCxxCdb.h"
#include "project/SourceGroupCxxEmpty.h"
#include "settings/SourceGroupSettingsCxxCdb.h"
#include "settings/SourceGroupSettingsCxxEmpty.h"

SourceGroupFactoryModuleCxx::~SourceGroupFactoryModuleCxx()
{
}

bool SourceGroupFactoryModuleCxx::supports(SourceGroupType type) const
{
	switch (type)
	{
	case SOURCE_GROUP_C_EMPTY:
	case SOURCE_GROUP_CPP_EMPTY:
	case SOURCE_GROUP_CXX_CDB:
		return true;
	default:
		break;
	}
	return false;
}

std::shared_ptr<SourceGroup> SourceGroupFactoryModuleCxx::createSourceGroup(std::shared_ptr<SourceGroupSettings> settings)
{
	std::shared_ptr<SourceGroup> sourceGroup;
	if (std::shared_ptr<SourceGroupSettingsCxxCdb> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(settings))
	{
		sourceGroup = std::shared_ptr<SourceGroup>(new SourceGroupCxxCdb(cxxSettings));
	}
	else if (std::shared_ptr<SourceGroupSettingsCxxEmpty> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxxEmpty>(settings))
	{
		sourceGroup = std::shared_ptr<SourceGroup>(new SourceGroupCxxEmpty(cxxSettings));
	}
	return sourceGroup;
}
