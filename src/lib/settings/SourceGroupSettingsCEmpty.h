#ifndef SOURCE_GROUP_SETTINGS_C_EMPTY_H
#define SOURCE_GROUP_SETTINGS_C_EMPTY_H

#include "settings/SourceGroupSettingsCxxEmpty.h"
#include "settings/SourceGroupSettingsWithCStandard.h"

class SourceGroupSettingsCEmpty
	: public SourceGroupSettingsCxxEmpty
	, public SourceGroupSettingsWithCStandard
{
public:
	SourceGroupSettingsCEmpty(const std::string& id, const ProjectSettings* projectSettings);

	void load(std::shared_ptr<const ConfigManager> config) override;
	void save(std::shared_ptr<ConfigManager> config) override;

	bool equals(std::shared_ptr<SourceGroupSettings> other) const override;

private:
	const ProjectSettings* getProjectSettings() const override;
	std::vector<std::wstring> getDefaultSourceExtensions() const override;
};

#endif // SOURCE_GROUP_SETTINGS_C_EMPTY_H
