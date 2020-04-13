#include "SourceGroupSettingsWithPythonEnvironmentPath.h"

#include "../../../ProjectSettings.h"
#include "../../../../utility/file/utilityFile.h"

FilePath SourceGroupSettingsWithPythonEnvironmentPath::getEnvironmentPath() const
{
	return m_environmentPath;
}

FilePath SourceGroupSettingsWithPythonEnvironmentPath::getEnvironmentPathExpandedAndAbsolute() const
{
	return utility::getExpandedAndAbsolutePath(
		getEnvironmentPath(), getProjectSettings()->getProjectDirectoryPath());
}

void SourceGroupSettingsWithPythonEnvironmentPath::setEnvironmentPath(const FilePath& environmentPath)
{
	m_environmentPath = environmentPath;
}

bool SourceGroupSettingsWithPythonEnvironmentPath::equals(const SourceGroupSettingsBase* other) const
{
	const SourceGroupSettingsWithPythonEnvironmentPath* otherPtr =
		dynamic_cast<const SourceGroupSettingsWithPythonEnvironmentPath*>(other);

	return (otherPtr && m_environmentPath == otherPtr->m_environmentPath);
}

void SourceGroupSettingsWithPythonEnvironmentPath::load(
	const ConfigManager* config, const std::string& key)
{
	setEnvironmentPath(config->getValueOrDefault(key + "/python_environment_path", FilePath(L"")));
}

void SourceGroupSettingsWithPythonEnvironmentPath::save(ConfigManager* config, const std::string& key)
{
	config->setValue(key + "/python_environment_path", getEnvironmentPath().wstr());
}
