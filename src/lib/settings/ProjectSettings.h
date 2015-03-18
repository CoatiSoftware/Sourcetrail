#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

#include <memory>
#include <vector>

#include "settings/CommonSettings.h"

class ProjectSettings
	: public CommonSettings
{
public:
	static std::shared_ptr<ProjectSettings> getInstance();
	~ProjectSettings();

	// source
	std::vector<std::string> getSourcePaths() const;
	bool setSourcePaths(const std::vector<std::string>& sourcePaths);

	// extensions
	std::vector<std::string> getHeaderExtensions() const;
	std::vector<std::string> getSourceExtensions() const;

private:
	ProjectSettings();
	ProjectSettings(const ProjectSettings&);
	void operator=(const ProjectSettings&);

	static std::shared_ptr<ProjectSettings> s_instance;
};

#endif // PROJECT_SETTINGS_H
