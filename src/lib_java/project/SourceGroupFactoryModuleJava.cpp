#include "project/SourceGroupFactoryModuleJava.h"

#include "project/SourceGroupJavaEmpty.h"
#include "project/SourceGroupJavaGradle.h"
#include "project/SourceGroupJavaMaven.h"
#include "project/SourceGroupJavaSonargraph.h"
#include "settings/SourceGroupSettingsJavaEmpty.h"
#include "settings/SourceGroupSettingsJavaGradle.h"
#include "settings/SourceGroupSettingsJavaMaven.h"
#include "settings/SourceGroupSettingsJavaSonargraph.h"

SourceGroupFactoryModuleJava::~SourceGroupFactoryModuleJava()
{
}

bool SourceGroupFactoryModuleJava::supports(SourceGroupType type) const
{
	switch (type)
	{
	case SOURCE_GROUP_JAVA_EMPTY:
	case SOURCE_GROUP_JAVA_GRADLE:
	case SOURCE_GROUP_JAVA_MAVEN:
	case SOURCE_GROUP_JAVA_SONARGRAPH:
		return true;
	default:
		break;
	}
	return false;
}

std::shared_ptr<SourceGroup> SourceGroupFactoryModuleJava::createSourceGroup(std::shared_ptr<SourceGroupSettings> settings)
{
	std::shared_ptr<SourceGroup> sourceGroup;
	if (std::shared_ptr<SourceGroupSettingsJavaEmpty> javaSettings = std::dynamic_pointer_cast<SourceGroupSettingsJavaEmpty>(settings))
	{
		sourceGroup = std::shared_ptr<SourceGroup>(new SourceGroupJavaEmpty(javaSettings));
	}
	else if (std::shared_ptr<SourceGroupSettingsJavaGradle> javaSettings = std::dynamic_pointer_cast<SourceGroupSettingsJavaGradle>(settings))
	{
		sourceGroup = std::shared_ptr<SourceGroup>(new SourceGroupJavaGradle(javaSettings));
	}
	else if (std::shared_ptr<SourceGroupSettingsJavaMaven> javaSettings = std::dynamic_pointer_cast<SourceGroupSettingsJavaMaven>(settings))
	{
		sourceGroup = std::shared_ptr<SourceGroup>(new SourceGroupJavaMaven(javaSettings));
	}
	else if (std::shared_ptr<SourceGroupSettingsJavaSonargraph> javaSettings = std::dynamic_pointer_cast<SourceGroupSettingsJavaSonargraph>(settings))
	{
		sourceGroup = std::shared_ptr<SourceGroup>(new SourceGroupJavaSonargraph(javaSettings));
	}
	return sourceGroup;
}
