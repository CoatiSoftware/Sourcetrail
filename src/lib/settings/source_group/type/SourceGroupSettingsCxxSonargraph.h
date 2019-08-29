#ifndef SOURCE_GROUP_SETTINGS_CXX_SONARGRAPH_H
#define SOURCE_GROUP_SETTINGS_CXX_SONARGRAPH_H

#include "SourceGroupSettingsWithComponents.h"
#include "SourceGroupSettingsWithCppStandard.h"
#include "SourceGroupSettingsWithCxxPathsAndFlags.h"
#include "SourceGroupSettingsWithIndexedHeaderPaths.h"
#include "SourceGroupSettingsWithSonargraphProjectPath.h"

class SourceGroupSettingsCxxSonargraph
	: public SourceGroupSettingsWithComponents<
		SourceGroupSettingsWithCppStandard,
		SourceGroupSettingsWithCxxPathsAndFlags,
		SourceGroupSettingsWithIndexedHeaderPaths,
		SourceGroupSettingsWithSonargraphProjectPath>
{
public:
	SourceGroupSettingsCxxSonargraph(const std::string& id, const ProjectSettings* projectSettings)
		: SourceGroupSettingsWithComponents(SOURCE_GROUP_CXX_SONARGRAPH, id, projectSettings)
	{
	}

	std::shared_ptr<SourceGroupSettings> createCopy() const override
	{
		return std::make_shared<SourceGroupSettingsCxxSonargraph>(*this);
	}
};

#endif // SOURCE_GROUP_SETTINGS_CXX_SONARGRAPH_H
