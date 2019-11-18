#ifndef MIGRATOR_H
#define MIGRATOR_H

#include <map>
#include <memory>

#include "Migration.h"

template <typename MigratableType>
class Migrator
{
public:
	virtual ~Migrator();

	void addMigration(size_t targetVersion, std::shared_ptr<Migration<MigratableType>> migration);

	bool willMigrate(const MigratableType* migratable, size_t targetVersion) const;
	bool migrate(MigratableType* migratable, size_t targetVersion) const;

private:
	typedef std::multimap<size_t, std::shared_ptr<Migration<MigratableType>>> MigrationMap;
	MigrationMap m_migrations;
};


template <typename MigratableType>
Migrator<MigratableType>::~Migrator()
{
}

template <typename MigratableType>
void Migrator<MigratableType>::addMigration(
	size_t targetVersion, std::shared_ptr<Migration<MigratableType>> migration)
{
	if (migration)
	{
		m_migrations.emplace(targetVersion, migration);
	}
}

template <typename MigratableType>
bool Migrator<MigratableType>::willMigrate(const MigratableType* migratable, size_t targetVersion) const
{
	size_t sourceVersion = migratable->getVersion();

	if (sourceVersion < targetVersion)
	{
		for (; sourceVersion <= targetVersion; sourceVersion++)
		{
			std::pair<typename MigrationMap::const_iterator, typename MigrationMap::const_iterator> ret;
			ret = m_migrations.equal_range(sourceVersion);

			for (typename MigrationMap::const_iterator it = ret.first; it != ret.second; it++)
			{
				return true;
			}
		}
	}

	return false;
}

template <typename MigratableType>
bool Migrator<MigratableType>::migrate(MigratableType* migratable, size_t targetVersion) const
{
	size_t sourceVersion = migratable->getVersion();

	if (sourceVersion < targetVersion)
	{
		for (; sourceVersion <= targetVersion; sourceVersion++)
		{
			std::pair<typename MigrationMap::const_iterator, typename MigrationMap::const_iterator> ret;
			ret = m_migrations.equal_range(sourceVersion);

			for (typename MigrationMap::const_iterator it = ret.first; it != ret.second; it++)
			{
				it->second->apply(migratable);
			}
		}

		migratable->setVersion(targetVersion);
		return true;
	}

	return false;
}

#endif	  // MIGRATOR_H
