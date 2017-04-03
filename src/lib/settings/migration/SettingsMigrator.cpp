#include "settings/migration/SettingsMigrator.h"

#include "settings/migration/Migration.h"
#include "settings/Settings.h"

SettingsMigrator::SettingsMigrator()
{
}

SettingsMigrator::~SettingsMigrator()
{
}

void SettingsMigrator::addMigration(size_t targetVersion, std::shared_ptr<Migration> migration)
{
	if (migration)
	{
		m_migrations.emplace(targetVersion, migration);
	}
}

bool SettingsMigrator::willMigrate(const Settings* settings, size_t targetVersion) const
{
	size_t originVersion = settings->getVersion();

	if (originVersion < targetVersion)
	{
		for (; originVersion <= targetVersion; originVersion++)
		{
			std::pair<std::multimap<size_t, std::shared_ptr<Migration>>::const_iterator, std::multimap<size_t, std::shared_ptr<Migration>>::const_iterator> ret;
			ret = m_migrations.equal_range(originVersion);

			for (std::multimap<size_t, std::shared_ptr<Migration>>::const_iterator it = ret.first; it != ret.second; it++)
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
			std::pair<std::multimap<size_t, std::shared_ptr<Migration>>::const_iterator, std::multimap<size_t, std::shared_ptr<Migration>>::const_iterator> ret;
			ret = m_migrations.equal_range(originVersion);

			for (std::multimap<size_t, std::shared_ptr<Migration>>::const_iterator it = ret.first; it != ret.second; it++)
			{
				it->second->apply(settings);
			}
		}

		settings->setVersion(targetVersion);
		return true;
	}

	return false;
}
