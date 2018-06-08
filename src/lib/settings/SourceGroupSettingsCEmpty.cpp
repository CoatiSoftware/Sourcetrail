#include "settings/SourceGroupSettingsCEmpty.h"

SourceGroupSettingsCEmpty::SourceGroupSettingsCEmpty(const std::string& id, const ProjectSettings* projectSettings)
	: SourceGroupSettingsCxxEmpty(id, SOURCE_GROUP_C_EMPTY, projectSettings)
{
}

void SourceGroupSettingsCEmpty::load(std::shared_ptr<const ConfigManager> config)
{
	SourceGroupSettingsCxxEmpty::load(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithCStandard::load(config, key);
}

void SourceGroupSettingsCEmpty::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettingsCxxEmpty::save(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithCStandard::save(config, key);
}

bool SourceGroupSettingsCEmpty::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsCEmpty> otherCxxEmpty = std::dynamic_pointer_cast<SourceGroupSettingsCEmpty>(other);

	return (
		otherCxxEmpty &&
		SourceGroupSettingsCxxEmpty::equals(other) &&
		SourceGroupSettingsWithCStandard::equals(otherCxxEmpty)
	);
}

const ProjectSettings* SourceGroupSettingsCEmpty::getProjectSettings() const
{
	return m_projectSettings;
}

std::vector<std::wstring> SourceGroupSettingsCEmpty::getDefaultSourceExtensions() const
{
	return { L".c" };
}
