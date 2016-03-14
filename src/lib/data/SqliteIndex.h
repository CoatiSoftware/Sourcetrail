#ifndef SQLITE_INDEX_H
#define SQLITE_INDEX_H

#include <string>
#include "sqlite/CppSQLite3.h"

class SqliteIndex
{
public:
	SqliteIndex(const std::string& indexName, const std::string& indexTarget);
	~SqliteIndex();

	void createOnDatabase(CppSQLite3DB& database);
	void removeFromDatabase(CppSQLite3DB& database);

private:
	std::string m_indexName;
	std::string m_indexTarget;
};

#endif // SQLITE_INDEX_H
