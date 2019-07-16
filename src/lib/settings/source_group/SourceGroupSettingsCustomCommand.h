#ifndef SOURCE_GROUP_SETTINGS_CUSTOM_COMMAND_H
#define SOURCE_GROUP_SETTINGS_CUSTOM_COMMAND_H

#include "SourceGroupSettings.h"
#include "SourceGroupSettingsWithExcludeFilters.h"
#include "SourceGroupSettingsWithSourceExtensions.h"
#include "SourceGroupSettingsWithSourcePaths.h"

class SourceGroupSettingsCustomCommand
	: public SourceGroupSettings
	, public SourceGroupSettingsWithExcludeFilters
	, public SourceGroupSettingsWithSourceExtensions
	, public SourceGroupSettingsWithSourcePaths
{
public:
	SourceGroupSettingsCustomCommand(const std::string& id, const ProjectSettings* projectSettings);

	std::shared_ptr<SourceGroupSettings> createCopy() const override;

	void load(std::shared_ptr<const ConfigManager> config) override;
	void save(std::shared_ptr<ConfigManager> config) override;

	bool equals(std::shared_ptr<SourceGroupSettings> other) const override;

	const std::wstring& getCustomCommand() const;
	void setCustomCommand(const std::wstring& customCommand);

	bool getRunInParallel() const;
	void setRunInParallel(bool runInParallel);

private:
	std::vector<std::wstring> getDefaultSourceExtensions() const override;

	std::wstring m_customCommand;
	bool m_runInParallel;
};

#endif // SOURCE_GROUP_SETTINGS_CUSTOM_COMMAND_H
