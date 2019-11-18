#include "SourceGroupSettingsWithCustomCommand.h"

#include "ConfigManager.h"

const std::wstring& SourceGroupSettingsWithCustomCommand::getCustomCommand() const
{
	return m_customCommand;
}

void SourceGroupSettingsWithCustomCommand::setCustomCommand(const std::wstring& customCommand)
{
	m_customCommand = customCommand;
}

bool SourceGroupSettingsWithCustomCommand::getRunInParallel() const
{
	return m_runInParallel;
}

void SourceGroupSettingsWithCustomCommand::setRunInParallel(bool runInParallel)
{
	m_runInParallel = runInParallel;
}

bool SourceGroupSettingsWithCustomCommand::equals(const SourceGroupSettingsBase* other) const
{
	const SourceGroupSettingsWithCustomCommand* otherPtr =
		dynamic_cast<const SourceGroupSettingsWithCustomCommand*>(other);

	return (otherPtr && m_customCommand == otherPtr->m_customCommand);
}

void SourceGroupSettingsWithCustomCommand::load(const ConfigManager* config, const std::string& key)
{
	setCustomCommand(config->getValueOrDefault(key + "/custom_command", std::wstring()));
	setRunInParallel(config->getValueOrDefault(key + "/run_in_parallel", false));
}

void SourceGroupSettingsWithCustomCommand::save(ConfigManager* config, const std::string& key)
{
	config->setValue(key + "/custom_command", getCustomCommand());
	config->setValue(key + "/run_in_parallel", getRunInParallel());
}
