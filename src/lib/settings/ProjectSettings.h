#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

#include <memory>
#include <vector>

#include "settings/Settings.h"

class ProjectSettings
	: public Settings
{
public:
	static std::vector<std::string> getDefaultHeaderExtensions();
	static std::vector<std::string> getDefaultSourceExtensions();

	static std::shared_ptr<ProjectSettings> getInstance();
	ProjectSettings();
	~ProjectSettings();

	bool operator==(const ProjectSettings& other) const;

	virtual void save(const FilePath& filePath);

	// language settings
	std::string getLanguage() const;
	bool setLanguage(const std::string& language);

	std::string getStandard() const;
	bool setStandard(const std::string& standard);

	// source
	std::vector<FilePath> getSourcePaths() const;
	std::vector<FilePath> getAbsoluteSourcePaths() const;
	bool setSourcePaths(const std::vector<FilePath>& sourcePaths);

	std::vector<FilePath> getHeaderSearchPaths() const;
	std::vector<FilePath> getAbsoluteHeaderSearchPaths() const;
	bool setHeaderSearchPaths(const std::vector<FilePath>& headerSearchPaths);

	std::vector<FilePath> getFrameworkSearchPaths() const;
	std::vector<FilePath> getAbsoluteFrameworkSearchPaths() const;
	bool setFrameworkSearchPaths(const std::vector<FilePath>& frameworkSearchPaths);

	std::vector<std::string> getCompilerFlags() const;
	bool setCompilerFlags(const std::vector<std::string>& compilerFlags);

	std::vector<std::string> getHeaderExtensions() const;
	std::vector<std::string> getSourceExtensions() const;

	bool setHeaderExtensions(const std::vector<std::string>& headerExtensions);
	bool setSourceExtensions(const std::vector<std::string>& sourceExtensions);

	bool isUseSourcePathsForHeaderSearchDefined() const;
	bool getUseSourcePathsForHeaderSearch() const;
	bool setUseSourcePathsForHeaderSearch(bool useSourcePathsForHeaderSearch);

	FilePath getVisualStudioSolutionPath() const;
	bool setVisualStudioSolutionPath(const FilePath& visualStudioSolutionPath);

	FilePath getCompilationDatabasePath() const;
	bool setCompilationDatabasePath(const FilePath& compilationDatabasePath);

	// info
	std::string getDescription() const;

	// used in project wizzard
	std::string getProjectName() const;
	void setProjectName(const std::string& name);

	std::string getProjectFileLocation() const;
	void setProjectFileLocation(const std::string& location);

private:
	void makePathsAbsolute(std::vector<FilePath>& paths) const;

	std::string m_projectName;
	std::string m_projectFileLocation;

	static std::shared_ptr<ProjectSettings> s_instance;
};

#endif // PROJECT_SETTINGS_H
