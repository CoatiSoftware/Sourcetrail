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
	return getValue<std::string>("SourcePath", "");
}

void ProjectSettings::setSourcePath(const std::string& sourcePath)
{
	setValue<std::string>("SourcePath", sourcePath);
}
