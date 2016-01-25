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
	defaultValues.push_back(".c");
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
	moveRelativePathValues("source/source_paths/source_path", filePath);
	moveRelativePathValues("source/header_search_paths/header_search_path", filePath);
	moveRelativePathValues("source/framework_search_paths/framework_search_path", filePath);

	Settings::save(filePath);
}

std::string ProjectSettings::getLanguage() const
{
	return getValue<std::string>("language_settings/language", "C++");
}

bool ProjectSettings::setLanguage(const std::string& language)
{
	return setValue<std::string>("language_settings/language", language);
}

std::string ProjectSettings::getStandard() const
{
	return getValue<std::string>("language_settings/standard", "11");
}

bool ProjectSettings::setStandard(const std::string& standard)
{
	return setValue<std::string>("language_settings/standard", standard);
}

std::vector<FilePath> ProjectSettings::getSourcePaths() const
{
	return getRelativePathValues("source/source_paths/source_path");
}

bool ProjectSettings::setSourcePaths(const std::vector<FilePath>& sourcePaths)
{
	return setPathValues("source/source_paths/source_path", sourcePaths);
}

std::vector<FilePath> ProjectSettings::getHeaderSearchPaths() const
{
	return getRelativePathValues("source/header_search_paths/header_search_path");
}

bool ProjectSettings::setHeaderSearchPaths(const std::vector<FilePath>& headerSearchPaths)
{
	return setPathValues("source/header_search_paths/header_search_path", headerSearchPaths);
}

std::vector<FilePath> ProjectSettings::getFrameworkSearchPaths() const
{
	return getRelativePathValues("source/framework_search_paths/framework_search_path");
}

bool ProjectSettings::setFrameworkSearchPaths(const std::vector<FilePath>& frameworkSearchPaths)
{
	return setPathValues("source/framework_search_paths/framework_search_path", frameworkSearchPaths);
}

std::vector<std::string> ProjectSettings::getCompilerFlags() const
{
	std::vector<std::string> defaultValues;
	return getValues("source/compiler_flags/compiler_flag", defaultValues);
}

std::vector<std::string> ProjectSettings::getHeaderExtensions() const
{
	return getValues("source/extensions/header_extensions", getDefaultHeaderExtensions());
}

std::vector<std::string> ProjectSettings::getSourceExtensions() const
{
	return getValues("source/extensions/source_extensions", getDefaultSourceExtensions());
}

bool ProjectSettings::setHeaderExtensions(const std::vector<std::string> &headerExtensions)
{
	return setValues("source/extensions/header_extensions", headerExtensions);
}

bool ProjectSettings::setSourceExtensions(const std::vector<std::string> &sourceExtensions)
{
	return setValues("source/extensions/source_extensions", sourceExtensions);
}

std::string ProjectSettings::getDescription() const
{
	return getValue<std::string>("info/description", "");
}
