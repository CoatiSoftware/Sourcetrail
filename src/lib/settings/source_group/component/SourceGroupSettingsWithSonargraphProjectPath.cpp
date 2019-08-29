#include "SourceGroupSettingsWithSonargraphProjectPath.h"

#include "ProjectSettings.h"
#include "utilityFile.h"

FilePath SourceGroupSettingsWithSonargraphProjectPath::getSonargraphProjectPath() const
{
	return m_sonargraphProjectPath;
}

FilePath SourceGroupSettingsWithSonargraphProjectPath::getSonargraphProjectPathExpandedAndAbsolute() const
{
	return utility::getExpandedAndAbsolutePath(getSonargraphProjectPath(), getProjectSettings()->getProjectDirectoryPath());
}

void SourceGroupSettingsWithSonargraphProjectPath::setSonargraphProjectPath(const FilePath& sonargraphProjectPath)
{
	m_sonargraphProjectPath = sonargraphProjectPath;
}

bool SourceGroupSettingsWithSonargraphProjectPath::equals(const SourceGroupSettingsBase* other) const
{
	const SourceGroupSettingsWithSonargraphProjectPath* otherPtr =
		dynamic_cast<const SourceGroupSettingsWithSonargraphProjectPath*>(other);

	return (
		otherPtr &&
		m_sonargraphProjectPath == otherPtr->m_sonargraphProjectPath
	);
}

void SourceGroupSettingsWithSonargraphProjectPath::load(const ConfigManager* config, const std::string& key)
{
	setSonargraphProjectPath(config->getValueOrDefault(key + "/sonargraph_project_path", FilePath(L"")));
}

void SourceGroupSettingsWithSonargraphProjectPath::save(ConfigManager* config, const std::string& key)
{
	config->setValue(key + "/sonargraph_project_path", getSonargraphProjectPath().wstr());
}
