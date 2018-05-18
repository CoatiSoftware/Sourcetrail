#include "settings/SourceGroupSettingsJavaSonargraph.h"

SourceGroupSettingsJavaSonargraph::SourceGroupSettingsJavaSonargraph(const std::string& id, const ProjectSettings* projectSettings)
	: SourceGroupSettings(id, SOURCE_GROUP_JAVA_SONARGRAPH, projectSettings)
{
}

void SourceGroupSettingsJavaSonargraph::load(std::shared_ptr<const ConfigManager> config)
{
	SourceGroupSettings::load(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithClasspath::load(config, key);
	SourceGroupSettingsWithSonargraphProjectPath::load(config, key);
}

void SourceGroupSettingsJavaSonargraph::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettings::save(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithClasspath::save(config, key);
	SourceGroupSettingsWithSonargraphProjectPath::save(config, key);
}

bool SourceGroupSettingsJavaSonargraph::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsJavaSonargraph> otherJavaSonargraph = std::dynamic_pointer_cast<SourceGroupSettingsJavaSonargraph>(other);

	return (
		otherJavaSonargraph &&
		SourceGroupSettings::equals(other) &&
		SourceGroupSettingsWithClasspath::equals(otherJavaSonargraph) &&
		SourceGroupSettingsWithSonargraphProjectPath::equals(otherJavaSonargraph)
	);
}

std::vector<std::string> SourceGroupSettingsJavaSonargraph::getAvailableLanguageStandards() const
{
	return std::vector<std::string>{"1", "2", "3", "4", "5", "6", "7", "8"};
}

std::string SourceGroupSettingsJavaSonargraph::getDefaultStandard() const
{
	return "8";
}

const ProjectSettings* SourceGroupSettingsJavaSonargraph::getProjectSettings() const
{
	return m_projectSettings;
}
