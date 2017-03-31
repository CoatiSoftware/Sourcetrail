#include "project/SourceGroupFactoryModuleCpp.h"

#include "project/SourceGroupCxx.h"
#include "settings/SourceGroupSettingsCxx.h"

SourceGroupFactoryModuleCpp::~SourceGroupFactoryModuleCpp()
{
}

bool SourceGroupFactoryModuleCpp::supports(SourceGroupType type) const
{
	switch (type)
	{
	case SOURCE_GROUP_CPP_EMPTY:
	case SOURCE_GROUP_CXX_CDB:
		return true;
	default:
		break;
	}
	return false;
}

std::shared_ptr<SourceGroup> SourceGroupFactoryModuleCpp::createSourceGroup(std::shared_ptr<SourceGroupSettings> settings)
{
	std::shared_ptr<SourceGroup> sourceGroup;
	if (std::shared_ptr<SourceGroupSettingsCxx> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(settings))
	{
		sourceGroup = std::shared_ptr<SourceGroup>(new SourceGroupCxx(cxxSettings));
	}
	return sourceGroup;
}
