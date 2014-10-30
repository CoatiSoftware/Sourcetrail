#include "ProjectSettings.h"

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

std::string ProjectSettings::getSourcePath() const
{
	return getValue<std::string>("source/SourcePath", "");
}

bool ProjectSettings::setSourcePath(const std::string& sourcePath)
{
	return setValue<std::string>("source/SourcePath", sourcePath);
}

std::vector<std::string> ProjectSettings::getHeaderSearchPaths() const
{
	//TODO defaultvalues?
	std::vector<std::string> defaultValues;
	return getValues("source/HeaderSearchPaths/HeaderSearchPath", defaultValues);
}
