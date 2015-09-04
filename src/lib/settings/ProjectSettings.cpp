#include "settings/ProjectSettings.h"

std::vector<std::string> ProjectSettings::getDefaultHeaderExtensions()
{
	std::vector<std::string> defaultValues;
	defaultValues.push_back(".h");
	defaultValues.push_back(".hpp");
	return defaultValues;
}

std::vector<std::string> ProjectSettings::getDefaultSourceExtensions()
{
	std::vector<std::string> defaultValues;
	defaultValues.push_back(".cpp");
	defaultValues.push_back(".cxx");
	defaultValues.push_back(".cc");
	return defaultValues;
}

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
	return getValues("source/Extensions/HeaderExtensions", getDefaultHeaderExtensions());
}

std::vector<std::string> ProjectSettings::getSourceExtensions() const
{
	return getValues("source/Extensions/SourceExtensions", getDefaultSourceExtensions());
}

bool ProjectSettings::setHeaderExtensions(const std::vector<std::string> &headerExtensions)
{
	return setValues("source/Extensions/HeaderExtensions", headerExtensions);
}

bool ProjectSettings::setSourceExtensions(const std::vector<std::string> &sourceExtensions)
{
	return setValues("source/Extensions/SourceExtensions", sourceExtensions);
}
