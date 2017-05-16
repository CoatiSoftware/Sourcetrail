#ifndef SETTINGS_MIGRATION_DELETE_KEY_H
#define SETTINGS_MIGRATION_DELETE_KEY_H

#include <string>

#include "settings/migration/SettingsMigration.h"

class SettingsMigrationDeleteKey: public SettingsMigration
{
public:
	SettingsMigrationDeleteKey(const std::string& key);
	virtual ~SettingsMigrationDeleteKey();
	virtual void apply(Settings* migratable) const;

private:
	const std::string m_key;
};

#endif // SETTINGS_MIGRATION_DELETE_KEY_H
