#include "settings/migration/Migration.h"

#include "settings/Settings.h"

Migration::~Migration()
{
}

bool Migration::isValueDefinedInSettings(const Settings* settings, const std::string& key) const
{
	return settings->isValueDefined(key);
}

void Migration::removeValuesInSettings(Settings* settings, const std::string& key) const
{
	settings->removeValues(key);
}
