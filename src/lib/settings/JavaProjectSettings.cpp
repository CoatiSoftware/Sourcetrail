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
	std::vector<FilePath> paths = getClasspaths();
	expandPaths(paths);
	makePathsAbsolute(paths);
	return paths;
}

bool JavaProjectSettings::setClasspaths(const std::vector<FilePath>& paths)
{
	return setPathValues("source/class_paths/class_path", paths);
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
