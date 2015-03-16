#include "settings/CommonSettings.h"

CommonSettings::~CommonSettings()
{
}

std::vector<std::string> CommonSettings::getHeaderSearchPaths() const
{
	std::vector<std::string> defaultValues;
	return getValues("source/HeaderSearchPaths/HeaderSearchPath", defaultValues);
}

std::vector<std::string> CommonSettings::getFrameworkSearchPaths() const
{
	std::vector<std::string> defaultValues;
	return getValues("source/FrameworkSearchPaths/FrameworkSearchPath", defaultValues);
}

std::vector<std::string> CommonSettings::getCompilerFlags() const
{
	std::vector<std::string> defaultValues;
	return getValues("source/CompilerFlags/CompilerFlag", defaultValues);
}

CommonSettings::CommonSettings()
{
}
