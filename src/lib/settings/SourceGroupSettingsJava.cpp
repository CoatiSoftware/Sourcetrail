#include "SourceGroupSettingsJava.h"

SourceGroupSettingsJava::SourceGroupSettingsJava(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings)
	: SourceGroupSettings(id, type, projectSettings)
{
}

void SourceGroupSettingsJava::load(std::shared_ptr<const ConfigManager> config)
{
	SourceGroupSettings::load(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithSourceExtensions::load(config, key);
	SourceGroupSettingsWithExcludeFilters::load(config, key);
	SourceGroupSettingsWithJavaStandard::load(config, key);
}

void SourceGroupSettingsJava::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettings::save(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithSourceExtensions::save(config, key);
	SourceGroupSettingsWithExcludeFilters::save(config, key);
	SourceGroupSettingsWithJavaStandard::save(config, key);
}

bool SourceGroupSettingsJava::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsJava> otherJava = std::dynamic_pointer_cast<SourceGroupSettingsJava>(other);

	return (
		otherJava &&
		SourceGroupSettings::equals(other) &&
		SourceGroupSettingsWithSourceExtensions::equals(otherJava) &&
		SourceGroupSettingsWithExcludeFilters::equals(otherJava) &&
		SourceGroupSettingsWithJavaStandard::equals(otherJava)
	);
}

std::vector<std::wstring> SourceGroupSettingsJava::getDefaultSourceExtensions() const
{
	return { L".java" };
}
