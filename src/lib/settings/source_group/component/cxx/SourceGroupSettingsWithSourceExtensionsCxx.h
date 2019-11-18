#ifndef SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_CXX_H
#define SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_CXX_H

#include "SourceGroupSettingsWithSourceExtensions.h"

class SourceGroupSettingsWithSourceExtensionsCxx: public SourceGroupSettingsWithSourceExtensions
{
private:
	std::vector<std::wstring> getDefaultSourceExtensions() const override
	{
		return {L".c", L".cpp", L".cxx", L".cc"};
	}
};

#endif	  // SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_CXX_H
