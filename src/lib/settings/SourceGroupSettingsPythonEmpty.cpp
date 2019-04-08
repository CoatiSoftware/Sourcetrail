#include "SourceGroupSettingsPythonEmpty.h"

#include "FilePath.h"
#include "ProjectSettings.h"

SourceGroupSettingsPythonEmpty::SourceGroupSettingsPythonEmpty(const std::string& id, const ProjectSettings* projectSettings)
	: SourceGroupSettings(id, SOURCE_GROUP_PYTHON_EMPTY, projectSettings)
{
}

std::shared_ptr<SourceGroupSettings> SourceGroupSettingsPythonEmpty::createCopy() const
{
	return std::make_shared<SourceGroupSettingsPythonEmpty>(*this);
}

void SourceGroupSettingsPythonEmpty::load(std::shared_ptr<const ConfigManager> config)
{
	SourceGroupSettings::load(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithExcludeFilters::load(config, key);
	SourceGroupSettingsWithSourcePaths::load(config, key);
	SourceGroupSettingsWithSourceExtensions::load(config, key);

	setEnvironmentDirectoryPath(config->getValueOrDefault(key + "/python_environment_directory_path", FilePath(L"")));
}

void SourceGroupSettingsPythonEmpty::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettings::save(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithExcludeFilters::save(config, key);
	SourceGroupSettingsWithSourcePaths::save(config, key);
	SourceGroupSettingsWithSourceExtensions::save(config, key);

	config->setValue(key + "/python_environment_directory_path", getEnvironmentDirectoryPath().wstr());
}

bool SourceGroupSettingsPythonEmpty::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsPythonEmpty> otherPython = std::dynamic_pointer_cast<SourceGroupSettingsPythonEmpty>(other);

	return (
		otherPython &&
		SourceGroupSettings::equals(other) &&
		SourceGroupSettingsWithExcludeFilters::equals(otherPython) &&
		SourceGroupSettingsWithSourcePaths::equals(otherPython) &&
		SourceGroupSettingsWithSourceExtensions::equals(otherPython) &&
		m_environmentDirectoryPath == otherPython->m_environmentDirectoryPath
	);
}

FilePath SourceGroupSettingsPythonEmpty::getEnvironmentDirectoryPath() const
{
	return m_environmentDirectoryPath;
}

FilePath SourceGroupSettingsPythonEmpty::getEnvironmentDirectoryPathExpandedAndAbsolute() const
{
	return m_projectSettings->makePathExpandedAndAbsolute(getEnvironmentDirectoryPath());
}

void SourceGroupSettingsPythonEmpty::setEnvironmentDirectoryPath(const FilePath& environmentDirectoryPath)
{
	m_environmentDirectoryPath = environmentDirectoryPath;
}

std::vector<std::wstring> SourceGroupSettingsPythonEmpty::getDefaultSourceExtensions() const
{
	return {
		L".py"
	};
}
