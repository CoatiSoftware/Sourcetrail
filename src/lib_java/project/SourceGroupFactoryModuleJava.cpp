#include "project/SourceGroupFactoryModuleJava.h"

#include "project/SourceGroupJava.h"

SourceGroupFactoryModuleJava::~SourceGroupFactoryModuleJava()
{
}

bool SourceGroupFactoryModuleJava::supports(SourceGroupType type) const
{
	switch (type)
	{
	case SOURCE_GROUP_JAVA_EMPTY:
	case SOURCE_GROUP_JAVA_MAVEN:
		return true;
	default:
		break;
	}
	return false;
}

std::shared_ptr<SourceGroup> SourceGroupFactoryModuleJava::createSourceGroup(std::shared_ptr<SourceGroupSettings> settings)
{
	std::shared_ptr<SourceGroup> sourceGroup;
	if (std::shared_ptr<SourceGroupSettingsJava> javaSettings = std::dynamic_pointer_cast<SourceGroupSettingsJava>(settings))
	{
		sourceGroup = std::shared_ptr<SourceGroup>(new SourceGroupJava(javaSettings));
	}
	return sourceGroup;
}
