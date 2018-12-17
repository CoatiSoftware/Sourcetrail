#include "SourceGroupSettingsCustomCommand.h"

#include "ConfigManager.h"
#include "FilePath.h"

SourceGroupSettingsCustomCommand::SourceGroupSettingsCustomCommand(
	const std::string& id, const ProjectSettings* projectSettings
)
	: SourceGroupSettings(id, SOURCE_GROUP_CUSTOM_COMMAND, projectSettings)
{
}

std::shared_ptr<SourceGroupSettings> SourceGroupSettingsCustomCommand::createCopy() const
{
	return std::make_shared<SourceGroupSettingsCustomCommand>(*this);
}

void SourceGroupSettingsCustomCommand::load(std::shared_ptr<const ConfigManager> config)
{
	SourceGroupSettings::load(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithSourceExtensions::load(config, key);
	SourceGroupSettingsWithSourcePaths::load(config, key);
	SourceGroupSettingsWithExcludeFilters::load(config, key);

	setCustomCommand(config->getValueOrDefault(key + "/custom_command", std::wstring()));
}

void SourceGroupSettingsCustomCommand::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettings::save(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithSourceExtensions::save(config, key);
	SourceGroupSettingsWithSourcePaths::save(config, key);
	SourceGroupSettingsWithExcludeFilters::save(config, key);

	config->setValue(key + "/custom_command", getCustomCommand());
}

bool SourceGroupSettingsCustomCommand::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsCustomCommand> otherCustom =
		std::dynamic_pointer_cast<SourceGroupSettingsCustomCommand>(other);

	return (
		otherCustom &&
		SourceGroupSettings::equals(other) &&
		SourceGroupSettingsWithSourceExtensions::equals(otherCustom) &&
		SourceGroupSettingsWithSourcePaths::equals(otherCustom) &&
		SourceGroupSettingsWithExcludeFilters::equals(otherCustom) &&
		m_customCommand == otherCustom->m_customCommand
	);
}

const std::wstring& SourceGroupSettingsCustomCommand::getCustomCommand() const
{
	return m_customCommand;
}

void SourceGroupSettingsCustomCommand::setCustomCommand(const std::wstring& customCommand)
{
	m_customCommand = customCommand;
}

std::vector<std::wstring> SourceGroupSettingsCustomCommand::getDefaultSourceExtensions() const
{
	return {};
}
