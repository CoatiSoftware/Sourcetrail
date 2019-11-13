#ifndef SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_CPP_H
#define SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_CPP_H

#include "language_packages.h"

#if BUILD_CXX_LANGUAGE_PACKAGE

#include "SourceGroupSettingsWithSourceExtensions.h"

class SourceGroupSettingsWithSourceExtensionsCpp
	: public SourceGroupSettingsWithSourceExtensions
{
private:
	std::vector<std::wstring> getDefaultSourceExtensions() const override
	{
		return {
			L".cpp",
			L".cxx",
			L".cc"
		};
	}
};

#endif // BUILD_CXX_LANGUAGE_PACKAGE

#endif // SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_CPP_H
