#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

#include <memory>
#include <vector>

#include "settings/Settings.h"

class ProjectSettings
	: public Settings
{
public:
	static std::shared_ptr<ProjectSettings> getInstance();
	~ProjectSettings();

	virtual void save(const FilePath& filePath);

	// source
	std::vector<FilePath> getSourcePaths() const;
	bool setSourcePaths(const std::vector<FilePath>& sourcePaths);

	std::vector<FilePath> getHeaderSearchPaths() const;
	bool setHeaderSearchPaths(const std::vector<FilePath>& headerSearchPaths);

	std::vector<FilePath> getFrameworkSearchPaths() const;
	bool setFrameworkSearchPaths(const std::vector<FilePath>& frameworkSearchPaths);

	std::vector<std::string> getCompilerFlags() const;

	// extensions
	std::vector<std::string> getHeaderExtensions() const;
	std::vector<std::string> getSourceExtensions() const;

	bool setHeaderExtensions(const std::vector<std::string>& headerExtensions);
	bool setSourceExtensions(const std::vector<std::string>& sourceExtensions);

private:
	ProjectSettings();
	ProjectSettings(const ProjectSettings&);
	void operator=(const ProjectSettings&);

	static std::shared_ptr<ProjectSettings> s_instance;
};

#endif // PROJECT_SETTINGS_H
