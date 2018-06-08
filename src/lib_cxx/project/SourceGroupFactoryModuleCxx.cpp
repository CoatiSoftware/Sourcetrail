#include "project/SourceGroupFactoryModuleCxx.h"

#include "project/SourceGroupCxxCdb.h"
#include "project/SourceGroupCxxCodeblocks.h"
#include "project/SourceGroupCxxEmpty.h"
#include "project/SourceGroupCxxSonargraph.h"
#include "settings/SourceGroupSettingsCxxCdb.h"
#include "settings/SourceGroupSettingsCxxCodeblocks.h"
#include "settings/SourceGroupSettingsCxxEmpty.h"
#include "settings/SourceGroupSettingsCxxSonargraph.h"

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
	case SOURCE_GROUP_CXX_CODEBLOCKS:
	case SOURCE_GROUP_CXX_SONARGRAPH:
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
	else if (std::shared_ptr<SourceGroupSettingsCxxCodeblocks> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxxCodeblocks>(settings))
	{
		sourceGroup = std::shared_ptr<SourceGroup>(new SourceGroupCxxCodeblocks(cxxSettings));
	}
	else if (std::shared_ptr<SourceGroupSettingsCxxEmpty> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxxEmpty>(settings))
	{
		sourceGroup = std::shared_ptr<SourceGroup>(new SourceGroupCxxEmpty(cxxSettings));
	}
	else if (std::shared_ptr<SourceGroupSettingsCxxSonargraph> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxxSonargraph>(settings))
	{
		sourceGroup = std::shared_ptr<SourceGroup>(new SourceGroupCxxSonargraph(cxxSettings));
	}
	return sourceGroup;
}
