#include "SettingsMigration.h"

#include "../Settings.h"

SettingsMigration::~SettingsMigration() {}

bool SettingsMigration::isValueDefinedInSettings(const Settings* settings, const std::string& key) const
{
	return settings->isValueDefined(key);
}

void SettingsMigration::removeValuesInSettings(Settings* settings, const std::string& key) const
{
	settings->removeValues(key);
}
