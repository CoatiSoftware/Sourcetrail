#ifndef SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_JAVA_H
#define SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_JAVA_H

#include "language_packages.h"

#if BUILD_JAVA_LANGUAGE_PACKAGE

#include "SourceGroupSettingsWithSourceExtensions.h"

class SourceGroupSettingsWithSourceExtensionsJava
	: public SourceGroupSettingsWithSourceExtensions
{
private:
	std::vector<std::wstring> getDefaultSourceExtensions() const override
	{
		return { L".java" };
	}
};

#endif // BUILD_JAVA_LANGUAGE_PACKAGE

#endif // SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_JAVA_H
