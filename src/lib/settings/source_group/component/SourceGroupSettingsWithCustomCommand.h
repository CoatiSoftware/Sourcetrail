#ifndef SOURCE_GROUP_SETTINGS_WITH_CUSTOM_COMMAND_H
#define SOURCE_GROUP_SETTINGS_WITH_CUSTOM_COMMAND_H

#include "SourceGroupSettingsComponent.h"

class SourceGroupSettingsWithCustomCommand
	: public SourceGroupSettingsComponent
{
public:
	virtual ~SourceGroupSettingsWithCustomCommand() = default;

	const std::wstring& getCustomCommand() const;
	void setCustomCommand(const std::wstring& customCommand);

	bool getRunInParallel() const;
	void setRunInParallel(bool runInParallel);

protected:
	bool equals(const SourceGroupSettingsBase* other) const override;

	void load(const ConfigManager* config, const std::string& key) override;
	void save(ConfigManager* config, const std::string& key) override;

private:
	std::wstring m_customCommand;
	bool m_runInParallel = false;
};

#endif // SOURCE_GROUP_SETTINGS_WITH_CUSTOM_COMMAND_H
