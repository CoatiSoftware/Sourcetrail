#ifndef SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_C_H
#define SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_C_H

#include "language_packages.h"

#if BUILD_CXX_LANGUAGE_PACKAGE

#include "SourceGroupSettingsWithSourceExtensions.h"

class SourceGroupSettingsWithSourceExtensionsC
	: public SourceGroupSettingsWithSourceExtensions
{
private:
	std::vector<std::wstring> getDefaultSourceExtensions() const override
	{
		return { L".c" };
	}
};

#endif // BUILD_CXX_LANGUAGE_PACKAGE

#endif // SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_C_H
