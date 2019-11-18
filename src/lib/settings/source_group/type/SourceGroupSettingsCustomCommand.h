#ifndef SOURCE_GROUP_SETTINGS_CUSTOM_COMMAND_H
#define SOURCE_GROUP_SETTINGS_CUSTOM_COMMAND_H

#include "SourceGroupSettingsWithComponents.h"
#include "SourceGroupSettingsWithCustomCommand.h"
#include "SourceGroupSettingsWithExcludeFilters.h"
#include "SourceGroupSettingsWithSourceExtensionsEmpty.h"
#include "SourceGroupSettingsWithSourcePaths.h"

class SourceGroupSettingsCustomCommand
	: public SourceGroupSettingsWithComponents<
		  SourceGroupSettingsWithCustomCommand,
		  SourceGroupSettingsWithExcludeFilters,
		  SourceGroupSettingsWithSourceExtensionsEmpty,
		  SourceGroupSettingsWithSourcePaths>
{
public:
	SourceGroupSettingsCustomCommand(const std::string& id, const ProjectSettings* projectSettings)
		: SourceGroupSettingsWithComponents(SOURCE_GROUP_CUSTOM_COMMAND, id, projectSettings)
	{
	}

	std::shared_ptr<SourceGroupSettings> createCopy() const override
	{
		return std::make_shared<SourceGroupSettingsCustomCommand>(*this);
	}
};

#endif	  // SOURCE_GROUP_SETTINGS_CUSTOM_COMMAND_H
