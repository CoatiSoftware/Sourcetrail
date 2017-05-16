#include "settings/migration/SettingsMigrationDeleteKey.h"

SettingsMigrationDeleteKey::SettingsMigrationDeleteKey(const std::string& key)
	: m_key(key)
{
}

SettingsMigrationDeleteKey::~SettingsMigrationDeleteKey()
{
}

void SettingsMigrationDeleteKey::apply(Settings* migratable) const
{
	removeValuesInSettings(migratable, m_key);
}
