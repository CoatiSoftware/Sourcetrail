#ifndef COMMON_SETTINGS_H
#define COMMON_SETTINGS_H

#include "settings/Settings.h"

class CommonSettings
	: public Settings
{
public:
	virtual ~CommonSettings();

	// source
	std::vector<std::string> getHeaderSearchPaths() const;
	std::vector<std::string> getFrameworkSearchPaths() const;
	std::vector<std::string> getCompilerFlags() const;

protected:
	CommonSettings();
};

#endif // COMMON_SETTINGS_H
