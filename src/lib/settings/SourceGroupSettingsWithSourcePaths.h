#ifndef SOURCE_GROUP_SETTINGS_WITH_SOURCE_PATHS_H
#define SOURCE_GROUP_SETTINGS_WITH_SOURCE_PATHS_H

#include <memory>
#include <string>
#include <vector>

class ConfigManager;
class FilePath;
class ProjectSettings;

class SourceGroupSettingsWithSourcePaths
{
public:
	SourceGroupSettingsWithSourcePaths();
	virtual ~SourceGroupSettingsWithSourcePaths() = default;
	
	bool equals(std::shared_ptr<SourceGroupSettingsWithSourcePaths> other) const;

	std::vector<FilePath> getSourcePaths() const;
	std::vector<FilePath> getSourcePathsExpandedAndAbsolute() const;
	void setSourcePaths(const std::vector<FilePath>& sourcePaths);

	std::vector<std::wstring> getSourceExtensions() const;
	void setSourceExtensions(const std::vector<std::wstring>& sourceExtensions);
	
protected:
	void load(std::shared_ptr<const ConfigManager> config, const std::string& key);
	void save(std::shared_ptr<ConfigManager> config, const std::string& key);

private:
	virtual const ProjectSettings* getProjectSettings() const = 0;
	virtual std::vector<std::wstring> getDefaultSourceExtensions() const = 0;

	std::vector<FilePath> m_sourcePaths;
	std::vector<std::wstring> m_sourceExtensions;
};

#endif // SOURCE_GROUP_SETTINGS_WITH_SOURCE_PATHS_H
