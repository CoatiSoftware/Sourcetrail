#ifndef SOURCE_GROUP_SETTINGS_PYTHON_EMPTY_H
#define SOURCE_GROUP_SETTINGS_PYTHON_EMPTY_H

#include "language_packages.h"

#if BUILD_PYTHON_LANGUAGE_PACKAGE

#include "SourceGroupSettingsWithComponents.h"
#include "SourceGroupSettingsWithExcludeFilters.h"
#include "SourceGroupSettingsWithPythonEnvironmentPath.h"
#include "SourceGroupSettingsWithSourceExtensionsPython.h"
#include "SourceGroupSettingsWithSourcePaths.h"

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

#endif // BUILD_PYTHON_LANGUAGE_PACKAGE

#endif // SOURCE_GROUP_SETTINGS_PYTHON_EMPTY_H
