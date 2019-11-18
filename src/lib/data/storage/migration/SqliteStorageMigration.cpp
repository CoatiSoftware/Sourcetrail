#include "SqliteStorageMigration.h"

SqliteStorageMigration::~SqliteStorageMigration() {}

bool SqliteStorageMigration::executeStatementInStorage(
	SqliteStorage* storage, const std::string& statement) const
{
	return storage->executeStatement(statement);
}
