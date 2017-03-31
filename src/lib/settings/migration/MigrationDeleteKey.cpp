#include "settings/migration/MigrationDeleteKey.h"

MigrationDeleteKey::MigrationDeleteKey(const std::string& key)
	: m_key(key)
{
}

void MigrationDeleteKey::apply(Settings* settings) const
{
	removeValuesInSettings(settings, m_key);
}
