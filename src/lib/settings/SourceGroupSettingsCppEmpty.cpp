#include "settings/SourceGroupSettingsCppEmpty.h"

SourceGroupSettingsCppEmpty::SourceGroupSettingsCppEmpty(const std::string& id, const ProjectSettings* projectSettings)
	: SourceGroupSettingsCxxEmpty(id, SOURCE_GROUP_CPP_EMPTY, projectSettings)
{
}

void SourceGroupSettingsCppEmpty::load(std::shared_ptr<const ConfigManager> config)
{
	SourceGroupSettingsCxxEmpty::load(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithCppStandard::load(config, key);
}

void SourceGroupSettingsCppEmpty::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettingsCxxEmpty::save(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithCppStandard::save(config, key);
}

bool SourceGroupSettingsCppEmpty::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsCppEmpty> otherCxxEmpty = std::dynamic_pointer_cast<SourceGroupSettingsCppEmpty>(other);

	return (
		otherCxxEmpty &&
		SourceGroupSettingsCxxEmpty::equals(other) &&
		SourceGroupSettingsWithCppStandard::equals(otherCxxEmpty)
	);
}

const ProjectSettings* SourceGroupSettingsCppEmpty::getProjectSettings() const
{
	return m_projectSettings;
}

std::vector<std::wstring> SourceGroupSettingsCppEmpty::getDefaultSourceExtensions() const
{
	return { 
		L".cpp",
		L".cxx",
		L".cc"
	};
}
