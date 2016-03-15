#include "settings/ProjectSettings.h"

#include "utility/utility.h"

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
	defaultValues.push_back(".c");
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

bool ProjectSettings::operator==(const ProjectSettings& other) const
{
	return
		getFilePath() == other.getFilePath() &&
		getLanguage() == other.getLanguage() &&
		getStandard() == other.getStandard() &&
		getVisualStudioSolutionPath() == other.getVisualStudioSolutionPath() &&
		getCompilationDatabasePath() == other.getCompilationDatabasePath() &&
		getUseSourcePathsForHeaderSearch() == other.getUseSourcePathsForHeaderSearch() &&
		utility::isPermutation<FilePath>(getSourcePaths(), other.getSourcePaths()) &&
		utility::isPermutation<FilePath>(getHeaderSearchPaths(), other.getHeaderSearchPaths()) &&
		utility::isPermutation<FilePath>(getFrameworkSearchPaths(), other.getFrameworkSearchPaths()) &&
		utility::isPermutation<std::string>(getCompilerFlags(), other.getCompilerFlags()) &&
		utility::isPermutation<std::string>(getHeaderExtensions(), other.getHeaderExtensions()) &&
		utility::isPermutation<std::string>(getSourceExtensions(), other.getSourceExtensions());
}

void ProjectSettings::save(const FilePath& filePath)
{
	m_projectName = "";
	m_projectFileLocation = "";

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
	return getValue<std::string>("language_settings/standard", "1z");
}

bool ProjectSettings::setStandard(const std::string& standard)
{
	return setValue<std::string>("language_settings/standard", standard);
}

std::vector<FilePath> ProjectSettings::getSourcePaths() const
{
	return getPathValues("source/source_paths/source_path");
}

std::vector<FilePath> ProjectSettings::getAbsoluteSourcePaths() const
{
	std::vector<FilePath> paths = getSourcePaths();
	makePathsAbsolute(paths);
	return paths;
}

bool ProjectSettings::setSourcePaths(const std::vector<FilePath>& sourcePaths)
{
	return setPathValues("source/source_paths/source_path", sourcePaths);
}

std::vector<FilePath> ProjectSettings::getHeaderSearchPaths() const
{
	return getPathValues("source/header_search_paths/header_search_path");
}

std::vector<FilePath> ProjectSettings::getAbsoluteHeaderSearchPaths() const
{
	std::vector<FilePath> paths = getHeaderSearchPaths();
	makePathsAbsolute(paths);
	return paths;
}

bool ProjectSettings::setHeaderSearchPaths(const std::vector<FilePath>& headerSearchPaths)
{
	return setPathValues("source/header_search_paths/header_search_path", headerSearchPaths);
}

std::vector<FilePath> ProjectSettings::getFrameworkSearchPaths() const
{
	return getPathValues("source/framework_search_paths/framework_search_path");
}

std::vector<FilePath> ProjectSettings::getAbsoluteFrameworkSearchPaths() const
{
	std::vector<FilePath> paths = getFrameworkSearchPaths();
	makePathsAbsolute(paths);
	return paths;
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

bool ProjectSettings::setCompilerFlags(const std::vector<std::string>& compilerFlags)
{
	return setValues("source/compiler_flags/compiler_flag", compilerFlags);
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

bool ProjectSettings::isUseSourcePathsForHeaderSearchDefined() const
{
	return isValueDefined("source/use_source_paths_for_header_search");
}

bool ProjectSettings::getUseSourcePathsForHeaderSearch() const
{
	return getValue<bool>("source/use_source_paths_for_header_search", false);
}

bool ProjectSettings::setUseSourcePathsForHeaderSearch(bool useSourcePathsForHeaderSearch)
{
	return setValue<bool>("source/use_source_paths_for_header_search", useSourcePathsForHeaderSearch);
}

FilePath ProjectSettings::getVisualStudioSolutionPath() const
{
	return FilePath(getValue<std::string>("source/build_file_path/vs_solution_path", ""));
}

bool ProjectSettings::setVisualStudioSolutionPath(const FilePath& visualStudioSolutionPath)
{
	return setValue<std::string>("source/build_file_path/vs_solution_path", visualStudioSolutionPath.str());
}

FilePath ProjectSettings::getCompilationDatabasePath() const
{
	return FilePath(getValue<std::string>("source/build_file_path/compilation_db_path", ""));
}

bool ProjectSettings::setCompilationDatabasePath(const FilePath& compilationDatabasePath)
{
	return setValue<std::string>("source/build_file_path/compilation_db_path", compilationDatabasePath.str());
}

std::string ProjectSettings::getDescription() const
{
	return getValue<std::string>("info/description", "");
}

std::string ProjectSettings::getProjectName() const
{
	if (m_projectName.size())
	{
		return m_projectName;
	}

	return getFilePath().withoutExtension().fileName();
}

void ProjectSettings::setProjectName(const std::string& name)
{
	m_projectName = name;
}

std::string ProjectSettings::getProjectFileLocation() const
{
	if (m_projectFileLocation.size())
	{
		return m_projectFileLocation;
	}

	return getFilePath().parentDirectory().str();
}

void ProjectSettings::setProjectFileLocation(const std::string& location)
{
	m_projectFileLocation = location;
}

void ProjectSettings::makePathsAbsolute(std::vector<FilePath>& paths) const
{
	FilePath basePath = getFilePath().parentDirectory();
	for (size_t i = 0; i < paths.size(); i++)
	{
		if (!paths[i].isAbsolute())
		{
			paths[i] = basePath.concat(paths[i]).canonical();
		}
	}
}
