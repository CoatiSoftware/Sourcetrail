#include "SqliteStorageMigrationLambda.h"


SqliteStorageMigrationLambda::SqliteStorageMigrationLambda(std::function<void(const SqliteStorageMigration*, SqliteStorage*)> m_lambda)
	: m_lambda(m_lambda)
{
}

SqliteStorageMigrationLambda::~SqliteStorageMigrationLambda()
{
}

void SqliteStorageMigrationLambda::apply(SqliteStorage* migratable) const
{
	m_lambda(this, migratable);
}
