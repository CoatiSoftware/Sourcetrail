#include "settings/SourceGroupSettingsJava.h"

SourceGroupSettingsJava::SourceGroupSettingsJava(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings)
	: SourceGroupSettings(id, type, projectSettings)
{
}

void SourceGroupSettingsJava::load(std::shared_ptr<const ConfigManager> config)
{
	SourceGroupSettings::load(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithSourcePaths::load(config, key);
	SourceGroupSettingsWithExcludeFilters::load(config, key);
	SourceGroupSettingsWithClasspath::load(config, key);
}

void SourceGroupSettingsJava::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettings::save(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithSourcePaths::save(config, key);
	SourceGroupSettingsWithExcludeFilters::save(config, key);
	SourceGroupSettingsWithClasspath::save(config, key);
}

bool SourceGroupSettingsJava::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsJava> otherJava = std::dynamic_pointer_cast<SourceGroupSettingsJava>(other);

	return (
		otherJava &&
		SourceGroupSettings::equals(other) &&
		SourceGroupSettingsWithSourcePaths::equals(otherJava) &&
		SourceGroupSettingsWithExcludeFilters::equals(otherJava) &&
		SourceGroupSettingsWithClasspath::equals(otherJava)
	);
}

std::vector<std::string> SourceGroupSettingsJava::getAvailableLanguageStandards() const
{
	return std::vector<std::string>{"1", "2", "3", "4", "5", "6", "7", "8"};
}

const ProjectSettings* SourceGroupSettingsJava::getProjectSettings() const
{
	return m_projectSettings;
}

std::vector<std::wstring> SourceGroupSettingsJava::getDefaultSourceExtensions() const
{
	return { L".java" };
}

std::string SourceGroupSettingsJava::getDefaultStandard() const
{
	return "8";
}
