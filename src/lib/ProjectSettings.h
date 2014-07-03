#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

#include <memory>

#include "Settings.h"

class ProjectSettings: public Settings
{
public:
	static std::shared_ptr<ProjectSettings> getInstance();
	~ProjectSettings();

	std::string getSourcePath() const;
	void setSourcePath(const std::string& sourcePath);

private:
	ProjectSettings();
	ProjectSettings(const ProjectSettings&);
	void operator=(const ProjectSettings&);

	static std::shared_ptr<ProjectSettings> s_instance;
};

#endif // PROJECT_SETTINGS_H
