#include "SourceGroupFactoryModuleJava.h"

#include "SourceGroupJavaEmpty.h"
#include "SourceGroupJavaGradle.h"
#include "SourceGroupJavaMaven.h"
#include "SourceGroupJavaSonargraph.h"
#include "SourceGroupSettingsJavaEmpty.h"
#include "SourceGroupSettingsJavaGradle.h"
#include "SourceGroupSettingsJavaMaven.h"
#include "SourceGroupSettingsJavaSonargraph.h"

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

std::shared_ptr<SourceGroup> SourceGroupFactoryModuleJava::createSourceGroup(std::shared_ptr<SourceGroupSettings> settings) const
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
