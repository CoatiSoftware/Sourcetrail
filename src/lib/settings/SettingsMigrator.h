#ifndef SETTNGS_MIGRATOR_H
#define SETTNGS_MIGRATOR_H

#include <map>
#include <string>

class Settings;

class SettingsMigrator
{
public:
	static SettingsMigrator createApplicationSettingsMigrator();

	SettingsMigrator();
	virtual ~SettingsMigrator();

	void addMigration(size_t targetVersion, std::string oldKey, std::string newKey);

	bool migrate(Settings* settings, size_t targetVersion) const;

private:
	struct Migration
	{
		size_t targetVersion;
		std::string oldKey;
		std::string newKey;
	};

	std::multimap<size_t, Migration> m_migrations;
};

#endif // SETTINGS_MIGRATOR_H
