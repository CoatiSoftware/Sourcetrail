#include "SourceGroupFactoryModuleCxx.h"

#include "SourceGroupCxxCdb.h"
#include "SourceGroupCxxCodeblocks.h"
#include "SourceGroupCxxEmpty.h"
#include "SourceGroupSettingsCEmpty.h"
#include "SourceGroupSettingsCppEmpty.h"
#include "SourceGroupSettingsCxxCdb.h"
#include "SourceGroupSettingsCxxCodeblocks.h"

bool SourceGroupFactoryModuleCxx::supports(SourceGroupType type) const
{
	switch (type)
	{
	case SOURCE_GROUP_C_EMPTY:
	case SOURCE_GROUP_CPP_EMPTY:
	case SOURCE_GROUP_CXX_CDB:
	case SOURCE_GROUP_CXX_CODEBLOCKS:
		return true;
	default:
		break;
	}
	return false;
}

std::shared_ptr<SourceGroup> SourceGroupFactoryModuleCxx::createSourceGroup(
	std::shared_ptr<SourceGroupSettings> settings) const
{
	std::shared_ptr<SourceGroup> sourceGroup;
	if (std::shared_ptr<SourceGroupSettingsCxxCdb> cxxSettings =
			std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(settings))
	{
		sourceGroup = std::shared_ptr<SourceGroup>(new SourceGroupCxxCdb(cxxSettings));
	}
	else if (
		std::shared_ptr<SourceGroupSettingsCxxCodeblocks> cxxSettings =
			std::dynamic_pointer_cast<SourceGroupSettingsCxxCodeblocks>(settings))
	{
		sourceGroup = std::shared_ptr<SourceGroup>(new SourceGroupCxxCodeblocks(cxxSettings));
	}
	else if (
		std::shared_ptr<SourceGroupSettingsCEmpty> cxxSettings =
			std::dynamic_pointer_cast<SourceGroupSettingsCEmpty>(settings))
	{
		sourceGroup = std::shared_ptr<SourceGroup>(new SourceGroupCxxEmpty(cxxSettings));
	}
	else if (
		std::shared_ptr<SourceGroupSettingsCppEmpty> cxxSettings =
			std::dynamic_pointer_cast<SourceGroupSettingsCppEmpty>(settings))
	{
		sourceGroup = std::shared_ptr<SourceGroup>(new SourceGroupCxxEmpty(cxxSettings));
	}
	return sourceGroup;
}
