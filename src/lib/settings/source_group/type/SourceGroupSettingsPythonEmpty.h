#ifndef SOURCE_GROUP_SETTINGS_PYTHON_EMPTY_H
#define SOURCE_GROUP_SETTINGS_PYTHON_EMPTY_H

#include "../SourceGroupSettingsWithComponents.h"
#include "../component/SourceGroupSettingsWithExcludeFilters.h"
#include "../component/python/SourceGroupSettingsWithPythonEnvironmentPath.h"
#include "../component/python/SourceGroupSettingsWithSourceExtensionsPython.h"
#include "../component/SourceGroupSettingsWithSourcePaths.h"

class SourceGroupSettingsPythonEmpty
	: public SourceGroupSettingsWithComponents<
		  SourceGroupSettingsWithExcludeFilters,
		  SourceGroupSettingsWithPythonEnvironmentPath,
		  SourceGroupSettingsWithSourceExtensionsPython,
		  SourceGroupSettingsWithSourcePaths>
{
public:
	SourceGroupSettingsPythonEmpty(const std::string& id, const ProjectSettings* projectSettings)
		: SourceGroupSettingsWithComponents(SOURCE_GROUP_PYTHON_EMPTY, id, projectSettings)
	{
	}

	std::shared_ptr<SourceGroupSettings> createCopy() const override
	{
		return std::make_shared<SourceGroupSettingsPythonEmpty>(*this);
	}
};

#endif	  // SOURCE_GROUP_SETTINGS_PYTHON_EMPTY_H
