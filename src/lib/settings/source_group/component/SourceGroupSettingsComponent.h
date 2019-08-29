#ifndef SOURCE_GROUP_SETTINGS_COMPONENT_H
#define SOURCE_GROUP_SETTINGS_COMPONENT_H

#include <string>

#include "SourceGroupSettingsBase.h"

class ConfigManager;

class SourceGroupSettingsComponent
	: virtual public SourceGroupSettingsBase
{
public:
	virtual ~SourceGroupSettingsComponent() = default;

protected:
	virtual void load(const ConfigManager* config, const std::string& key) = 0;
	virtual void save(ConfigManager* config, const std::string& key) = 0;

	virtual bool equals(const SourceGroupSettingsBase* other) const = 0;
};

#endif // SOURCE_GROUP_SETTINGS_COMPONENT_H
