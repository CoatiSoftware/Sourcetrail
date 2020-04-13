#ifndef SOURCE_GROUP_SETTINGS_JAVA_GRADLE_H
#define SOURCE_GROUP_SETTINGS_JAVA_GRADLE_H

#include "../SourceGroupSettingsWithComponents.h"
#include "../component/SourceGroupSettingsWithExcludeFilters.h"
#include "../component/java/SourceGroupSettingsWithJavaGradle.h"
#include "../component/java/SourceGroupSettingsWithJavaStandard.h"
#include "../component/java/SourceGroupSettingsWithSourceExtensionsJava.h"

class SourceGroupSettingsJavaGradle
	: public SourceGroupSettingsWithComponents<
		  SourceGroupSettingsWithExcludeFilters,
		  SourceGroupSettingsWithJavaGradle,
		  SourceGroupSettingsWithJavaStandard,
		  SourceGroupSettingsWithSourceExtensionsJava>
{
public:
	SourceGroupSettingsJavaGradle(const std::string& id, const ProjectSettings* projectSettings)
		: SourceGroupSettingsWithComponents(SOURCE_GROUP_JAVA_GRADLE, id, projectSettings)
	{
	}

	std::shared_ptr<SourceGroupSettings> createCopy() const override
	{
		return std::make_shared<SourceGroupSettingsJavaGradle>(*this);
	}
};

#endif	  // SOURCE_GROUP_SETTINGS_JAVA_GRADLE_H
