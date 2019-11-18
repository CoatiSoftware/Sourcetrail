#ifndef SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_CPP_H
#define SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_CPP_H

#include "SourceGroupSettingsWithSourceExtensions.h"

class SourceGroupSettingsWithSourceExtensionsCpp: public SourceGroupSettingsWithSourceExtensions
{
private:
	std::vector<std::wstring> getDefaultSourceExtensions() const override
	{
		return {L".cpp", L".cxx", L".cc"};
	}
};

#endif	  // SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_CPP_H
