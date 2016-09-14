#include "settings/CxxProjectSettings.h"

#include "utility/utility.h"

CxxProjectSettings::CxxProjectSettings()
{
	//	setlanguage...
}

CxxProjectSettings::CxxProjectSettings(const FilePath& projectFilePath)
	: ProjectSettings(projectFilePath)
{
	//	setlanguage...
}

CxxProjectSettings::CxxProjectSettings(std::string projectName, const FilePath& projectFileLocation)
	: ProjectSettings(projectName, projectFileLocation)
{
	//	setlanguage...
}

CxxProjectSettings::~CxxProjectSettings()
{
}

bool CxxProjectSettings::equalsExceptNameAndLocation(const ProjectSettings& other) const
{
	if (other.getLanguage() == getLanguage())
	{
		// const CxxProjectSettings* typedOther = dynamic_cast<const CxxProjectSettings*>(&other);
		return(
			ProjectSettings::equalsExceptNameAndLocation(other)// &&
		//	utility::isPermutation<FilePath>(getClasspaths(), typedOther->getClasspaths())
		);
	}
	return false;
}

std::vector<std::string> CxxProjectSettings::getLanguageStandards() const
{
	std::vector<std::string> standards;

	switch (getLanguage())
	{
		case LANGUAGE_CPP:
			standards.push_back("c++1z");
			standards.push_back("gnu++1z");

			standards.push_back("c++14");
			standards.push_back("gnu++14");

			standards.push_back("c++1y");
			standards.push_back("gnu++1y");

			standards.push_back("c++11");
			standards.push_back("gnu++11");

			standards.push_back("c++0x");
			standards.push_back("gnu++0x");

			standards.push_back("c++03");

			standards.push_back("c++98");
			standards.push_back("gnu++98");
			break;

		case LANGUAGE_C:
			standards.push_back("c1x");
			standards.push_back("gnu1x");
			standards.push_back("iso9899:201x");

			standards.push_back("c11");
			standards.push_back("gnu11");
			standards.push_back("iso9899:2011");

			standards.push_back("c9x");
			standards.push_back("gnu9x");
			standards.push_back("iso9899:199x");

			standards.push_back("c99");
			standards.push_back("gnu99");
			standards.push_back("iso9899:1999");

			standards.push_back("iso9899:199409");

			standards.push_back("c90");
			standards.push_back("gnu90");
			standards.push_back("iso9899:1990");

			standards.push_back("c89");
			standards.push_back("gnu89");
			break;

		default:
			break;
	}

	return standards;
}

std::vector<FilePath> CxxProjectSettings::getHeaderSearchPaths() const
{
	return getPathValues("source/header_search_paths/header_search_path");
}

std::vector<FilePath> CxxProjectSettings::getAbsoluteHeaderSearchPaths() const
{
	std::vector<FilePath> paths = getHeaderSearchPaths();
	expandPaths(paths);
	makePathsAbsolute(paths);
	return paths;
}

bool CxxProjectSettings::setHeaderSearchPaths(const std::vector<FilePath>& headerSearchPaths)
{
	return setPathValues("source/header_search_paths/header_search_path", headerSearchPaths);
}

std::vector<FilePath> CxxProjectSettings::getFrameworkSearchPaths() const
{
	return getPathValues("source/framework_search_paths/framework_search_path");
}

std::vector<FilePath> CxxProjectSettings::getAbsoluteFrameworkSearchPaths() const
{
	std::vector<FilePath> paths = getFrameworkSearchPaths();
	expandPaths(paths);
	makePathsAbsolute(paths);
	return paths;
}

bool CxxProjectSettings::setFrameworkSearchPaths(const std::vector<FilePath>& frameworkSearchPaths)
{
	return setPathValues("source/framework_search_paths/framework_search_path", frameworkSearchPaths);
}

std::vector<std::string> CxxProjectSettings::getCompilerFlags() const
{
	std::vector<std::string> defaultValues;
	return getValues("source/compiler_flags/compiler_flag", defaultValues);
}

bool CxxProjectSettings::setCompilerFlags(const std::vector<std::string>& compilerFlags)
{
	return setValues("source/compiler_flags/compiler_flag", compilerFlags);
}

bool CxxProjectSettings::getUseSourcePathsForHeaderSearch() const
{
	return getValue<bool>("source/use_source_paths_for_header_search", false);
}

bool CxxProjectSettings::setUseSourcePathsForHeaderSearch(bool useSourcePathsForHeaderSearch)
{
	return setValue<bool>("source/use_source_paths_for_header_search", useSourcePathsForHeaderSearch);
}

FilePath CxxProjectSettings::getVisualStudioSolutionPath() const
{
	return FilePath(getValue<std::string>("source/build_file_path/vs_solution_path", ""));
}

bool CxxProjectSettings::setVisualStudioSolutionPath(const FilePath& visualStudioSolutionPath)
{
	return setValue<std::string>("source/build_file_path/vs_solution_path", visualStudioSolutionPath.str());
}

FilePath CxxProjectSettings::getCompilationDatabasePath() const
{
	return FilePath(getValue<std::string>("source/build_file_path/compilation_db_path", ""));
}

bool CxxProjectSettings::setCompilationDatabasePath(const FilePath& compilationDatabasePath)
{
	return setValue<std::string>("source/build_file_path/compilation_db_path", compilationDatabasePath.str());
}

std::vector<std::string> CxxProjectSettings::getDefaultSourceExtensions() const
{
	std::vector<std::string> defaultValues;
	defaultValues.push_back(".c");
	defaultValues.push_back(".cpp");
	defaultValues.push_back(".cxx");
	defaultValues.push_back(".cc");
	return defaultValues;
}

std::string CxxProjectSettings::getDefaultStandard() const
{
	return "1z";
}
