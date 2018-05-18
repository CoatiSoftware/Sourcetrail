#include "settings/SourceGroupSettingsWithSonargraphProjectPath.h"

#include "settings/ProjectSettings.h"

SourceGroupSettingsWithSonargraphProjectPath::SourceGroupSettingsWithSonargraphProjectPath()
{
}

bool SourceGroupSettingsWithSonargraphProjectPath::equals(std::shared_ptr<SourceGroupSettingsWithSonargraphProjectPath> other) const
{
	return (
		other &&
		m_sonargraphProjectPath == other->m_sonargraphProjectPath
	);
}

void SourceGroupSettingsWithSonargraphProjectPath::load(std::shared_ptr<const ConfigManager> config, const std::string& key)
{
	setSonargraphProjectPath(config->getValueOrDefault(key + "/sonargraph_project_path", FilePath(L"")));
}

void SourceGroupSettingsWithSonargraphProjectPath::save(std::shared_ptr<ConfigManager> config, const std::string& key)
{
	config->setValue(key + "/sonargraph_project_path", getSonargraphProjectPath().wstr());
}

FilePath SourceGroupSettingsWithSonargraphProjectPath::getSonargraphProjectPath() const
{
	return m_sonargraphProjectPath;
}

FilePath SourceGroupSettingsWithSonargraphProjectPath::getSonargraphProjectPathExpandedAndAbsolute() const
{
	return getProjectSettings()->makePathExpandedAndAbsolute(getSonargraphProjectPath());
}

void SourceGroupSettingsWithSonargraphProjectPath::setSonargraphProjectPath(const FilePath& sonargraphProjectPath)
{
	m_sonargraphProjectPath = sonargraphProjectPath;
}
