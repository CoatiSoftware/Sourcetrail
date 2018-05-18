#ifndef SOURCE_GROUP_SETTINGS_JAVA_EMPTY_H
#define SOURCE_GROUP_SETTINGS_JAVA_EMPTY_H

#include "settings/SourceGroupSettingsJava.h"

class SourceGroupSettingsJavaEmpty
	: public SourceGroupSettingsJava
{
public:
	SourceGroupSettingsJavaEmpty(const std::string& id, const ProjectSettings* projectSettings);
};

#endif // SOURCE_GROUP_SETTINGS_JAVA_EMPTY_H
