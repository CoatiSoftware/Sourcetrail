#include "settings/ProjectSettings.h"

std::shared_ptr<ProjectSettings> ProjectSettings::s_instance;

std::shared_ptr<ProjectSettings> ProjectSettings::getInstance()
{
	if (!s_instance)
	{
		s_instance = std::shared_ptr<ProjectSettings>(new ProjectSettings());
	}

	return s_instance;
}

ProjectSettings::ProjectSettings()
{
}

ProjectSettings::~ProjectSettings()
{
}

std::vector<std::string> ProjectSettings::getSourcePaths() const
{
	std::vector<std::string> defaultValues;
	return getValues("source/SourcePaths/SourcePath", defaultValues);
}

bool ProjectSettings::setSourcePaths(const std::vector<std::string>& sourcePaths)
{
	return setValues("source/SourcePaths/SourcePath", sourcePaths);
}

std::vector<std::string> ProjectSettings::getHeaderExtensions() const
{
	std::vector<std::string> defaultValues;
	return getValues("source/Extensions/HeaderExtensions", defaultValues);
}

std::vector<std::string> ProjectSettings::getSourceExtensions() const
{
	std::vector<std::string> defaultValues;
	return getValues("source/Extensions/SourceExtensions", defaultValues);
}