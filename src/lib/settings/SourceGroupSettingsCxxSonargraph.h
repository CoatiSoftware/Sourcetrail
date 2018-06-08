#ifndef SOURCE_GROUP_SETTINGS_CXX_SONARGRAPH_H
#define SOURCE_GROUP_SETTINGS_CXX_SONARGRAPH_H

#include "settings/SourceGroupSettingsCxx.h"
#include "settings/SourceGroupSettingsWithCppStandard.h"
#include "settings/SourceGroupSettingsWithIndexedHeaderPaths.h"
#include "settings/SourceGroupSettingsWithSonargraphProjectPath.h"

class SourceGroupSettingsCxxSonargraph
	: public SourceGroupSettingsCxx
	, public SourceGroupSettingsWithCppStandard
	, public SourceGroupSettingsWithIndexedHeaderPaths
	, public SourceGroupSettingsWithSonargraphProjectPath
{
public:
	SourceGroupSettingsCxxSonargraph(const std::string& id, const ProjectSettings* projectSettings);

	void load(std::shared_ptr<const ConfigManager> config) override;
	void save(std::shared_ptr<ConfigManager> config) override;

	bool equals(std::shared_ptr<SourceGroupSettings> other) const override;

private:
	const ProjectSettings* getProjectSettings() const override;
};

#endif // SOURCE_GROUP_SETTINGS_CXX_SONARGRAPH_H
