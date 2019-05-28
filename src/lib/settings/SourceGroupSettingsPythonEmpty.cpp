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

	setEnvironmentPath(config->getValueOrDefault(key + "/python_environment_path", FilePath(L"")));
}

void SourceGroupSettingsPythonEmpty::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettings::save(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithExcludeFilters::save(config, key);
	SourceGroupSettingsWithSourcePaths::save(config, key);
	SourceGroupSettingsWithSourceExtensions::save(config, key);

	config->setValue(key + "/python_environment_path", getEnvironmentPath().wstr());
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
		m_environmentPath == otherPython->m_environmentPath
	);
}

FilePath SourceGroupSettingsPythonEmpty::getEnvironmentPath() const
{
	return m_environmentPath;
}

FilePath SourceGroupSettingsPythonEmpty::getEnvironmentPathExpandedAndAbsolute() const
{
	return m_projectSettings->makePathExpandedAndAbsolute(getEnvironmentPath());
}

void SourceGroupSettingsPythonEmpty::setEnvironmentPath(const FilePath& environmentPath)
{
	m_environmentPath = environmentPath;
}

std::vector<std::wstring> SourceGroupSettingsPythonEmpty::getDefaultSourceExtensions() const
{
	return {
		L".py"
	};
}
