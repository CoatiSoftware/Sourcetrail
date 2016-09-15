#include "settings/SettingsMigrator.h"

#include "settings/Settings.h"

SettingsMigrator::SettingsMigrator()
{
}

SettingsMigrator::~SettingsMigrator()
{
}

void SettingsMigrator::addMigration(size_t targetVersion, std::string oldKey, std::string newKey)
{
	Migration migration;
	migration.targetVersion = targetVersion;
	migration.oldKey = oldKey;
	migration.newKey = newKey;

	m_migrations.emplace(targetVersion, migration);
}

void SettingsMigrator::addLambdaMigration(size_t targetVersion, std::function<void(Settings*)> lambda)
{
	Migration migration;
	migration.targetVersion = targetVersion;
	migration.lambda = lambda;

	m_migrations.emplace(targetVersion, migration);
}

bool SettingsMigrator::willMigrate(const Settings* settings, size_t targetVersion) const
{
	size_t originVersion = settings->getVersion();

	if (originVersion < targetVersion)
	{
		for (; originVersion <= targetVersion; originVersion++)
		{
			std::pair<std::multimap<size_t, Migration>::const_iterator, std::multimap<size_t, Migration>::const_iterator> ret;
			ret = m_migrations.equal_range(originVersion);

			for (std::multimap<size_t, Migration>::const_iterator it = ret.first; it != ret.second; it++)
			{
				return true;
			}
		}
	}

	return false;
}

bool SettingsMigrator::migrate(Settings* settings, size_t targetVersion) const
{
	size_t originVersion = settings->getVersion();

	if (originVersion < targetVersion)
	{
		for (; originVersion <= targetVersion; originVersion++)
		{
			std::pair<std::multimap<size_t, Migration>::const_iterator, std::multimap<size_t, Migration>::const_iterator> ret;
			ret = m_migrations.equal_range(originVersion);

			for (std::multimap<size_t, Migration>::const_iterator it = ret.first; it != ret.second; it++)
			{
				const Migration& migration = it->second;

				if (!migration.oldKey.size())
				{
					migration.lambda(settings);
				}
				else if (!settings->isValueDefined(migration.newKey))
				{
					settings->setValues<std::string>(
						migration.newKey,
						settings->getValues<std::string>(migration.oldKey, std::vector<std::string>())
					);
					settings->removeValues(migration.oldKey);
				}
			}
		}

		settings->setVersion(targetVersion);

		return true;
	}

	return false;
}
