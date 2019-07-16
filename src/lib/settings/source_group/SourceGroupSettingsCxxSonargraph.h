#ifndef SOURCE_GROUP_SETTINGS_CXX_SONARGRAPH_H
#define SOURCE_GROUP_SETTINGS_CXX_SONARGRAPH_H

#include "SourceGroupSettingsCxx.h"
#include "SourceGroupSettingsWithCppStandard.h"
#include "SourceGroupSettingsWithIndexedHeaderPaths.h"
#include "SourceGroupSettingsWithSonargraphProjectPath.h"

class SourceGroupSettingsCxxSonargraph
	: public SourceGroupSettingsCxx
	, public SourceGroupSettingsWithCppStandard
	, public SourceGroupSettingsWithIndexedHeaderPaths
	, public SourceGroupSettingsWithSonargraphProjectPath
{
public:
	SourceGroupSettingsCxxSonargraph(const std::string& id, const ProjectSettings* projectSettings);

	std::shared_ptr<SourceGroupSettings> createCopy() const override;

	void load(std::shared_ptr<const ConfigManager> config) override;
	void save(std::shared_ptr<ConfigManager> config) override;

	bool equals(std::shared_ptr<SourceGroupSettings> other) const override;
};

#endif // SOURCE_GROUP_SETTINGS_CXX_SONARGRAPH_H
