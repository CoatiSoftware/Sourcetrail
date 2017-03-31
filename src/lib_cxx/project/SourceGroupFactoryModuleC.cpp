#include "project/SourceGroupFactoryModuleC.h"

#include "project/SourceGroupCxx.h"
#include "settings/SourceGroupSettingsCxx.h"

SourceGroupFactoryModuleC::~SourceGroupFactoryModuleC()
{
}

bool SourceGroupFactoryModuleC::supports(SourceGroupType type) const
{
	switch (type)
	{
	case SOURCE_GROUP_C_EMPTY:
		return true;
	default:
		break;
	}
	return false;
}

std::shared_ptr<SourceGroup> SourceGroupFactoryModuleC::createSourceGroup(std::shared_ptr<SourceGroupSettings> settings)
{
	std::shared_ptr<SourceGroup> sourceGroup;
	if (std::shared_ptr<SourceGroupSettingsCxx> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(settings))
	{
		sourceGroup = std::shared_ptr<SourceGroup>(new SourceGroupCxx(cxxSettings));
	}
	return sourceGroup;
}
