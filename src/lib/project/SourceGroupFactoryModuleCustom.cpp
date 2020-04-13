#include "SourceGroupFactoryModuleCustom.h"

#include "SourceGroupCustomCommand.h"
#include "../settings/source_group/type/SourceGroupSettingsCustomCommand.h"

bool SourceGroupFactoryModuleCustom::supports(SourceGroupType type) const
{
	switch (type)
	{
	case SOURCE_GROUP_CUSTOM_COMMAND:
		return true;
	default:
		break;
	}
	return false;
}

std::shared_ptr<SourceGroup> SourceGroupFactoryModuleCustom::createSourceGroup(
	std::shared_ptr<SourceGroupSettings> settings) const
{
	std::shared_ptr<SourceGroup> sourceGroup;
	if (std::shared_ptr<SourceGroupSettingsCustomCommand> customSettings =
			std::dynamic_pointer_cast<SourceGroupSettingsCustomCommand>(settings))
	{
		sourceGroup = std::shared_ptr<SourceGroup>(new SourceGroupCustomCommand(customSettings));
	}
	return sourceGroup;
}
