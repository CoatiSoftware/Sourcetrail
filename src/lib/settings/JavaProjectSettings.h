#ifndef JAVA_PROJECT_SETTINGS_H
#define JAVA_PROJECT_SETTINGS_H

#include "settings/ProjectSettings.h"

class JavaProjectSettings
	: public ProjectSettings
{
public:
	JavaProjectSettings();
	JavaProjectSettings(const FilePath& projectFilePath);
	JavaProjectSettings(std::string projectName, const FilePath& projectFileLocation);
	virtual ~JavaProjectSettings();

	virtual bool equalsExceptNameAndLocation(const ProjectSettings& other) const;

	virtual std::vector<std::string> getLanguageStandards() const;

	std::vector<FilePath> getClasspaths() const;
	std::vector<FilePath> getAbsoluteClasspaths() const;
	bool setClasspaths(const std::vector<FilePath>& headerSearchPaths);

private:
	virtual std::vector<std::string> getDefaultSourceExtensions() const;
	virtual std::string getDefaultStandard() const;
};

#endif // JAVA_PROJECT_SETTINGS_H
