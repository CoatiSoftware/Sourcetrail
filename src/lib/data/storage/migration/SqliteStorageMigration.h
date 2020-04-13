#ifndef SQLITE_STORAGE_MIGRATION_H
#define SQLITE_STORAGE_MIGRATION_H

#include <string>
#include <vector>

#include "../../../utility/migration/Migration.h"
#include "../sqlite/SqliteStorage.h"

class SqliteStorageMigration: public Migration<SqliteStorage>
{
public:
	virtual ~SqliteStorageMigration();

	bool executeStatementInStorage(SqliteStorage* storage, const std::string& statement) const;
};

#endif	  // SQLITE_STORAGE_MIGRATION_H
