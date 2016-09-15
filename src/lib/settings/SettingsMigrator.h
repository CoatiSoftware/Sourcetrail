#ifndef SETTNGS_MIGRATOR_H
#define SETTNGS_MIGRATOR_H

#include <functional>
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
	void addLambdaMigration(size_t targetVersion, std::function<void(Settings*)> lambda);

	bool willMigrate(const Settings* settings, size_t targetVersion) const;
	bool migrate(Settings* settings, size_t targetVersion) const;

private:
	struct Migration
	{
		size_t targetVersion;
		std::string oldKey;
		std::string newKey;

		std::function<void(Settings*)> lambda;
	};

	std::multimap<size_t, Migration> m_migrations;
};

#endif // SETTINGS_MIGRATOR_H
