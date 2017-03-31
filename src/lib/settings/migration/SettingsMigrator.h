#ifndef SETTNGS_MIGRATOR_H
#define SETTNGS_MIGRATOR_H

#include <map>
#include <memory>
#include <string>

class Migration;
class Settings;

class SettingsMigrator
{
public:
	static SettingsMigrator createApplicationSettingsMigrator();

	SettingsMigrator();
	virtual ~SettingsMigrator();

	void addMigration(size_t targetVersion, std::shared_ptr<Migration> migration);

	bool willMigrate(const Settings* settings, size_t targetVersion) const;
	bool migrate(Settings* settings, size_t targetVersion) const;

private:
	std::multimap<size_t, std::shared_ptr<Migration>> m_migrations;
};

#endif // SETTINGS_MIGRATOR_H
