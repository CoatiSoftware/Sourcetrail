#ifndef SQLITE_DATABASE_INDEX_H
#define SQLITE_DATABASE_INDEX_H

#include <string>
#include "sqlite/CppSQLite3.h"

class SqliteDatabaseIndex
{
public:
	SqliteDatabaseIndex(const std::string& indexName, const std::string& indexTarget);
	~SqliteDatabaseIndex();

	void createOnDatabase(CppSQLite3DB& database);
	void removeFromDatabase(CppSQLite3DB& database);

private:
	std::string m_indexName;
	std::string m_indexTarget;
};

#endif // SQLITE_DATABASE_INDEX_H
