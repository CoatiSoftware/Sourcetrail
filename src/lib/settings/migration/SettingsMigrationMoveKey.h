#ifndef SETTINGS_MIGRATION_MOVE_KEY_H
#define SETTINGS_MIGRATION_MOVE_KEY_H

#include "SettingsMigration.h"

class SettingsMigrationMoveKey: public SettingsMigration
{
public:
	SettingsMigrationMoveKey(const std::string& oldKey, const std::string& newKey);
	virtual ~SettingsMigrationMoveKey();
	virtual void apply(Settings* migratable) const;

private:
	const std::string m_oldKey;
	const std::string m_newKey;
};

#endif	  // SETTINGS_MIGRATION_MOVE_KEY_H
