#include "SourceGroupSettingsPythonEmpty.h"
#include "FilePath.h"

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
}

void SourceGroupSettingsPythonEmpty::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettings::save(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithExcludeFilters::save(config, key);
	SourceGroupSettingsWithSourcePaths::save(config, key);
	SourceGroupSettingsWithSourceExtensions::save(config, key);
}

bool SourceGroupSettingsPythonEmpty::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsPythonEmpty> otherJava = std::dynamic_pointer_cast<SourceGroupSettingsPythonEmpty>(other);

	return (
		otherJava &&
		SourceGroupSettings::equals(other) &&
		SourceGroupSettingsWithExcludeFilters::equals(otherJava) &&
		SourceGroupSettingsWithSourcePaths::equals(otherJava) &&
		SourceGroupSettingsWithSourceExtensions::equals(otherJava)
	);
}

std::vector<std::wstring> SourceGroupSettingsPythonEmpty::getDefaultSourceExtensions() const
{
	return {
		L".py"
	};
}
