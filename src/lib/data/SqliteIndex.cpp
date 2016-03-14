#include "data/SqliteIndex.h"

SqliteIndex::SqliteIndex(const std::string& indexName, const std::string& indexTarget)
	: m_indexName(indexName)
	, m_indexTarget(indexTarget)
{
}

SqliteIndex::~SqliteIndex()
{
}

void SqliteIndex::createOnDatabase(CppSQLite3DB& database)
{
	database.execDML((
		"CREATE INDEX IF NOT EXISTS " + m_indexName + " ON " + m_indexTarget + ";"
	).c_str());
}

void SqliteIndex::removeFromDatabase(CppSQLite3DB& database)
{
	database.execDML((
		"DROP INDEX IF EXISTS main." + m_indexName + ";"
	).c_str());
}
