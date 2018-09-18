#ifndef SOURCE_GROUP_SETTINGS_WITH_SONARGRAPH_PROJECT_PATH_H
#define SOURCE_GROUP_SETTINGS_WITH_SONARGRAPH_PROJECT_PATH_H

#include <memory>
#include <string>

#include "FilePath.h"

class ConfigManager;
class ProjectSettings;

class SourceGroupSettingsWithSonargraphProjectPath
{
public:
	SourceGroupSettingsWithSonargraphProjectPath();
	virtual ~SourceGroupSettingsWithSonargraphProjectPath() = default;

	bool equals(std::shared_ptr<SourceGroupSettingsWithSonargraphProjectPath> other) const;

	FilePath getSonargraphProjectPath() const;
	FilePath getSonargraphProjectPathExpandedAndAbsolute() const;
	void setSonargraphProjectPath(const FilePath& sonargraphProjectPath);

protected:
	void load(std::shared_ptr<const ConfigManager> config, const std::string& key);
	void save(std::shared_ptr<ConfigManager> config, const std::string& key);

private:
	virtual const ProjectSettings* getProjectSettings() const = 0;

	FilePath m_sonargraphProjectPath;
};

#endif // SOURCE_GROUP_SETTINGS_WITH_SONARGRAPH_PROJECT_PATH_H
