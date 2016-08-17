#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

#include <memory>
#include <vector>

#include "settings/Settings.h"
#include "settings/LanguageType.h"

class ProjectSettings
	: public Settings
{
public:
	static LanguageType getLanguageOfProject(FilePath projectFilePath);

	ProjectSettings();
	ProjectSettings(const FilePath& projectFilePath);
	ProjectSettings(std::string projectName, const FilePath& projectFileLocation);
	virtual ~ProjectSettings();

	virtual bool equalsExceptNameAndLocation(const ProjectSettings& other) const;

	virtual std::vector<std::string> getLanguageStandards() const;

	bool reload();

	std::string getProjectName() const;
	void setProjectName(const std::string& name);

	FilePath getProjectFileLocation() const;
	void setProjectFileLocation(const FilePath& location);

	std::string getDescription() const;

	LanguageType getLanguage() const;
	bool setLanguage(LanguageType language);

	std::string getStandard() const;
	bool setStandard(const std::string& standard);

	std::vector<FilePath> getSourcePaths() const;
	std::vector<FilePath> getAbsoluteSourcePaths() const;
	bool setSourcePaths(const std::vector<FilePath>& sourcePaths);

	std::vector<FilePath> getExcludePaths() const;
	std::vector<FilePath> getAbsoluteExcludePaths() const;
	bool setExcludePaths(const std::vector<FilePath>& excludePaths);

	std::vector<std::string> getSourceExtensions() const;
	bool setSourceExtensions(const std::vector<std::string>& sourceExtensions);

protected:
	void makePathsAbsolute(std::vector<FilePath>& paths) const;

private:
	virtual std::vector<std::string> getDefaultSourceExtensions() const;
	virtual std::string getDefaultStandard() const;
};

#endif // PROJECT_SETTINGS_H
