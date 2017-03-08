#include "settings/JavaProjectSettings.h"

#include "utility/utility.h"

JavaProjectSettings::JavaProjectSettings()
{
	//	setlanguage...
}

JavaProjectSettings::JavaProjectSettings(const FilePath& projectFilePath)
	: ProjectSettings(projectFilePath)
{
	//	setlanguage...
}

JavaProjectSettings::JavaProjectSettings(std::string projectName, const FilePath& projectFileLocation)
	: ProjectSettings(projectName, projectFileLocation)
{
	//	setlanguage...
}

JavaProjectSettings::~JavaProjectSettings()
{
}

ProjectType JavaProjectSettings::getProjectType() const
{
	return PROJECT_JAVA_EMPTY;
}

bool JavaProjectSettings::equalsExceptNameAndLocation(const ProjectSettings& other) const
{
	if (other.getLanguage() == getLanguage())
	{
		const JavaProjectSettings* typedOther = dynamic_cast<const JavaProjectSettings*>(&other);
		return(
			ProjectSettings::equalsExceptNameAndLocation(other) &&
			utility::isPermutation<FilePath>(getClasspaths(), typedOther->getClasspaths())
		);
	}
	return false;
}

std::vector<std::string> JavaProjectSettings::getLanguageStandards() const
{
	return std::vector<std::string>(1, "8");
}

std::vector<FilePath> JavaProjectSettings::getClasspaths() const
{
	return getPathValues("source/class_paths/class_path");
}

std::vector<FilePath> JavaProjectSettings::getAbsoluteClasspaths() const
{
	return makePathsAbsolute(expandPaths(getClasspaths()));
}

bool JavaProjectSettings::setClasspaths(const std::vector<FilePath>& classpaths)
{
	return setPathValues("source/class_paths/class_path", classpaths);
}

FilePath JavaProjectSettings::getMavenProjectFilePath() const
{
	return FilePath(getValue<std::string>("source/maven/project_root", ""));
}

FilePath JavaProjectSettings::getAbsoluteMavenProjectFilePath() const
{
	return makePathAbsolute(expandPath(getMavenProjectFilePath()));
}

bool JavaProjectSettings::setMavenProjectFilePath(const FilePath& path)
{
	return setValue("source/maven/project_root", path.str());
}

FilePath JavaProjectSettings::getMavenDependenciesDirectory() const
{
	return FilePath(getValue<std::string>("source/maven/dependencies_directory", ""));
}

FilePath JavaProjectSettings::getAbsoluteMavenDependenciesDirectory() const
{
	return makePathAbsolute(expandPath(getMavenDependenciesDirectory()));
}

bool JavaProjectSettings::setMavenDependenciesDirectory(const FilePath& path)
{
	return setValue("source/maven/dependencies_directory", path.str());
}

bool JavaProjectSettings::getShouldIndexMavenTests() const
{
	return getValue<bool>("source/maven/should_index_tests", false);
}
void JavaProjectSettings::setShouldIndexMavenTests(bool value)
{
	setValue<bool>("source/maven/should_index_tests", value);
}

std::vector<std::string> JavaProjectSettings::getDefaultSourceExtensions() const
{
	std::vector<std::string> defaultValues;
	defaultValues.push_back(".java");
	return defaultValues;
}

std::string JavaProjectSettings::getDefaultStandard() const
{
	return "8";
}
