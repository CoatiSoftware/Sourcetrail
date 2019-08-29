#ifndef SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_EMPTY_H
#define SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_EMPTY_H

#include "SourceGroupSettingsWithSourceExtensions.h"

class SourceGroupSettingsWithSourceExtensionsEmpty
	: public SourceGroupSettingsWithSourceExtensions
{
private:
	std::vector<std::wstring> getDefaultSourceExtensions() const override
	{
		return {};
	}
};

#endif // SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_EMPTY_H
