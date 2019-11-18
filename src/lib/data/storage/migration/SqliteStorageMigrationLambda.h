#ifndef SQLITE_STORAGE_MIGRATION_LAMBDA_H
#define SQLITE_STORAGE_MIGRATION_LAMBDA_H

#include <functional>

#include "SqliteStorageMigration.h"

class SqliteStorageMigrationLambda: public SqliteStorageMigration
{
public:
	SqliteStorageMigrationLambda(
		std::function<void(const SqliteStorageMigration*, SqliteStorage*)> m_lambda);
	virtual ~SqliteStorageMigrationLambda();
	virtual void apply(SqliteStorage* migratable) const;

private:
	std::function<void(const SqliteStorageMigration*, SqliteStorage*)> m_lambda;
};

#endif	  // SQLITE_STORAGE_MIGRATION_LAMBDA_H
