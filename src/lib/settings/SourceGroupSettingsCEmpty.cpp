#include "SourceGroupSettingsCEmpty.h"

SourceGroupSettingsCEmpty::SourceGroupSettingsCEmpty(const std::string& id, const ProjectSettings* projectSettings)
	: SourceGroupSettingsCxx(id, SOURCE_GROUP_C_EMPTY, projectSettings)
{
}

std::shared_ptr<SourceGroupSettings> SourceGroupSettingsCEmpty::createCopy() const
{
	return std::make_shared<SourceGroupSettingsCEmpty>(*this);
}

void SourceGroupSettingsCEmpty::load(std::shared_ptr<const ConfigManager> config)
{
	SourceGroupSettingsCxx::load(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithCStandard::load(config, key);
	SourceGroupSettingsWithCxxCrossCompilationOptions::load(config, key);
	SourceGroupSettingsWithSourceExtensions::load(config, key);
	SourceGroupSettingsWithSourcePaths::load(config, key);
	SourceGroupSettingsWithExcludeFilters::load(config, key);
}

void SourceGroupSettingsCEmpty::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettingsCxx::save(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithCStandard::save(config, key);
	SourceGroupSettingsWithCxxCrossCompilationOptions::save(config, key);
	SourceGroupSettingsWithSourceExtensions::save(config, key);
	SourceGroupSettingsWithSourcePaths::save(config, key);
	SourceGroupSettingsWithExcludeFilters::save(config, key);
}

bool SourceGroupSettingsCEmpty::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsCEmpty> otherCxxEmpty = std::dynamic_pointer_cast<SourceGroupSettingsCEmpty>(other);

	return (
		otherCxxEmpty &&
		SourceGroupSettingsCxx::equals(other) &&
		SourceGroupSettingsWithCStandard::equals(otherCxxEmpty) &&
		SourceGroupSettingsWithCxxCrossCompilationOptions::equals(otherCxxEmpty) &&
		SourceGroupSettingsWithSourceExtensions::equals(otherCxxEmpty) &&
		SourceGroupSettingsWithSourcePaths::equals(otherCxxEmpty) &&
		SourceGroupSettingsWithExcludeFilters::equals(otherCxxEmpty)
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
