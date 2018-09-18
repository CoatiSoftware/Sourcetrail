#include "SourceGroupSettingsCppEmpty.h"

SourceGroupSettingsCppEmpty::SourceGroupSettingsCppEmpty(const std::string& id, const ProjectSettings* projectSettings)
	: SourceGroupSettingsCxx(id, SOURCE_GROUP_CPP_EMPTY, projectSettings)
{
}

std::shared_ptr<SourceGroupSettings> SourceGroupSettingsCppEmpty::createCopy() const
{
	return std::make_shared<SourceGroupSettingsCppEmpty>(*this);
}

void SourceGroupSettingsCppEmpty::load(std::shared_ptr<const ConfigManager> config)
{
	SourceGroupSettingsCxx::load(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithCppStandard::load(config, key);
	SourceGroupSettingsWithCxxCrossCompilationOptions::load(config, key);
	SourceGroupSettingsWithSourceExtensions::load(config, key);
	SourceGroupSettingsWithSourcePaths::load(config, key);
	SourceGroupSettingsWithExcludeFilters::load(config, key);
}

void SourceGroupSettingsCppEmpty::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettingsCxx::save(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithCppStandard::save(config, key);
	SourceGroupSettingsWithCxxCrossCompilationOptions::save(config, key);
	SourceGroupSettingsWithSourceExtensions::save(config, key);
	SourceGroupSettingsWithSourcePaths::save(config, key);
	SourceGroupSettingsWithExcludeFilters::save(config, key);
}

bool SourceGroupSettingsCppEmpty::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsCppEmpty> otherCxxEmpty = std::dynamic_pointer_cast<SourceGroupSettingsCppEmpty>(other);

	return (
		otherCxxEmpty &&
		SourceGroupSettingsCxx::equals(other) &&
		SourceGroupSettingsWithCppStandard::equals(otherCxxEmpty) &&
		SourceGroupSettingsWithCxxCrossCompilationOptions::equals(otherCxxEmpty) &&
		SourceGroupSettingsWithSourceExtensions::equals(otherCxxEmpty) &&
		SourceGroupSettingsWithSourcePaths::equals(otherCxxEmpty) &&
		SourceGroupSettingsWithExcludeFilters::equals(otherCxxEmpty)
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
