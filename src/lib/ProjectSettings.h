#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

#include <string>
#include <memory.h>

#include "utility/ConfigManager.h"

class ProjectSettings
{
public:
	static std::shared_ptr<ProjectSettings> getInstance();
	~ProjectSettings();

	bool load(const std::string& projectSettingsFilePath);
	void save(const std::string& projectSettingsFilePath);

std::string getSourcePath() const;

	void setSourcePath(const std::string& sourcePath);

private:
	static std::shared_ptr<ProjectSettings> s_instance;
	ProjectSettings();
	ProjectSettings(const ProjectSettings&);
	void operator=(const ProjectSettings&);

	std::shared_ptr<ConfigManager> m_config;
};

#endif // PROJECT_SETTINGS_H
