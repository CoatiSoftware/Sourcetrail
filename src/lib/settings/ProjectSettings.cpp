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

void ProjectSettings::save(const FilePath& filePath)
{
	moveRelativePathValues("source/SourcePaths/SourcePath", filePath);
	moveRelativePathValues("source/HeaderSearchPaths/HeaderSearchPath", filePath);
	moveRelativePathValues("source/FrameworkSearchPaths/FrameworkSearchPath", filePath);

	Settings::save(filePath);
}

std::vector<FilePath> ProjectSettings::getSourcePaths() const
{
	return getRelativePathValues("source/SourcePaths/SourcePath");
}

bool ProjectSettings::setSourcePaths(const std::vector<FilePath>& sourcePaths)
{
	return setPathValues("source/SourcePaths/SourcePath", sourcePaths);
}

std::vector<FilePath> ProjectSettings::getHeaderSearchPaths() const
{
	return getRelativePathValues("source/HeaderSearchPaths/HeaderSearchPath");
}

bool ProjectSettings::setHeaderSearchPaths(const std::vector<FilePath>& headerSearchPaths)
{
	return setPathValues("source/HeaderSearchPaths/HeaderSearchPath", headerSearchPaths);
}

std::vector<FilePath> ProjectSettings::getFrameworkSearchPaths() const
{
	return getRelativePathValues("source/FrameworkSearchPaths/FrameworkSearchPath");
}

bool ProjectSettings::setFrameworkSearchPaths(const std::vector<FilePath>& frameworkSearchPaths)
{
	return setPathValues("source/FrameworkSearchPaths/FrameworkSearchPath", frameworkSearchPaths);
}

std::vector<std::string> ProjectSettings::getCompilerFlags() const
{
	std::vector<std::string> defaultValues;
	return getValues("source/CompilerFlags/CompilerFlag", defaultValues);
}

std::vector<std::string> ProjectSettings::getHeaderExtensions() const
{
	std::vector<std::string> defaultValues;
	defaultValues.push_back(".h");
	return getValues("source/Extensions/HeaderExtensions", defaultValues);
}

std::vector<std::string> ProjectSettings::getSourceExtensions() const
{
	std::vector<std::string> defaultValues;
	defaultValues.push_back(".cpp");
	return getValues("source/Extensions/SourceExtensions", defaultValues);
}
