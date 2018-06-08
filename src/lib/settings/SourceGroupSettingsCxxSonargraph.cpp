#include "settings/SourceGroupSettingsCxxSonargraph.h"

SourceGroupSettingsCxxSonargraph::SourceGroupSettingsCxxSonargraph(const std::string& id, const ProjectSettings* projectSettings)
	: SourceGroupSettingsCxx(id, SOURCE_GROUP_CXX_SONARGRAPH, projectSettings)
{
}

void SourceGroupSettingsCxxSonargraph::load(std::shared_ptr<const ConfigManager> config)
{
	SourceGroupSettingsCxx::load(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithCppStandard::load(config, key);
	SourceGroupSettingsWithIndexedHeaderPaths::load(config, key);
	SourceGroupSettingsWithSonargraphProjectPath::load(config, key);
}

void SourceGroupSettingsCxxSonargraph::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettingsCxx::save(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithCppStandard::save(config, key);
	SourceGroupSettingsWithIndexedHeaderPaths::save(config, key);
	SourceGroupSettingsWithSonargraphProjectPath::save(config, key);
}

bool SourceGroupSettingsCxxSonargraph::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsCxxSonargraph> otherCxxSonargraph = std::dynamic_pointer_cast<SourceGroupSettingsCxxSonargraph>(other);

	return (
		otherCxxSonargraph &&
		SourceGroupSettingsCxx::equals(other) &&
		SourceGroupSettingsWithCppStandard::equals(otherCxxSonargraph) &&
		SourceGroupSettingsWithIndexedHeaderPaths::equals(otherCxxSonargraph) &&
		SourceGroupSettingsWithSonargraphProjectPath::equals(otherCxxSonargraph)
	);
}

const ProjectSettings* SourceGroupSettingsCxxSonargraph::getProjectSettings() const
{
	return m_projectSettings;
}
