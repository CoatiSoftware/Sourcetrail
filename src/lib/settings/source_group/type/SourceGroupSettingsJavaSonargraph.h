#ifndef SOURCE_GROUP_SETTINGS_JAVA_SONARGRAPH_H
#define SOURCE_GROUP_SETTINGS_JAVA_SONARGRAPH_H

#include "SourceGroupSettingsWithComponents.h"
#include "SourceGroupSettingsWithClasspath.h"
#include "SourceGroupSettingsWithJavaStandard.h"
#include "SourceGroupSettingsWithSonargraphProjectPath.h"

class SourceGroupSettingsJavaSonargraph
	: public SourceGroupSettingsWithComponents<
		SourceGroupSettingsWithClasspath,
		SourceGroupSettingsWithJavaStandard,
		SourceGroupSettingsWithSonargraphProjectPath>
{
public:
	SourceGroupSettingsJavaSonargraph(const std::string& id, const ProjectSettings* projectSettings)
		: SourceGroupSettingsWithComponents(SOURCE_GROUP_JAVA_SONARGRAPH, id, projectSettings)
	{
	}

	std::shared_ptr<SourceGroupSettings> createCopy() const override
	{
		return std::make_shared<SourceGroupSettingsJavaSonargraph>(*this);
	}
};

#endif // SOURCE_GROUP_SETTINGS_JAVA_SONARGRAPH_H
