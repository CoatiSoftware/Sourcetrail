#ifndef SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_C_H
#define SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_C_H

#include "../SourceGroupSettingsWithSourceExtensions.h"

class SourceGroupSettingsWithSourceExtensionsC: public SourceGroupSettingsWithSourceExtensions
{
private:
	std::vector<std::wstring> getDefaultSourceExtensions() const override
	{
		return {L".c"};
	}
};

#endif	  // SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_C_H
