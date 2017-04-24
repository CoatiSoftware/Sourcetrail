#ifndef SQLITE_STORAGE_H
#define SQLITE_STORAGE_H

#include "sqlite/CppSQLite3.h"

#include "data/SqliteDatabaseIndex.h"
#include "utility/file/FilePath.h"
#include "utility/Version.h"

class SqliteStorage
{
public:
	enum StorageModeType
	{
		STORAGE_MODE_UNKNOWN = 0,
		STORAGE_MODE_READ = 1,
		STORAGE_MODE_WRITE = 2,
		STORAGE_MODE_CLEAR = 4,
	};

	SqliteStorage(const FilePath& dbFilePath);
	virtual ~SqliteStorage();

	void setup();
	void clear();

	void setMode(const StorageModeType mode);

	void beginTransaction();
	void commitTransaction();
	void rollbackTransaction();

	void optimizeMemory() const;

	FilePath getDbFilePath() const;

	bool isEmpty() const;
	bool isIncompatible() const;

	void setVersion();

protected:
	void setupMetaTable();
	void clearMetaTable();

	bool executeStatement(const std::string& statement) const;
	bool executeStatement(CppSQLite3Statement& statement) const;
	int executeStatementScalar(const std::string& statement) const;
	CppSQLite3Query executeQuery(const std::string& statement) const;
	CppSQLite3Query executeQuery(CppSQLite3Statement& statement) const;

	bool hasTable(const std::string& tableName) const;

	std::string getMetaValue(const std::string& key) const;
	void insertOrUpdateMetaValue(const std::string& key, const std::string& value);

	size_t getStorageVersion() const;
	void setStorageVersion();

	Version getApplicationVersion() const;
	void setApplicationVersion();

	mutable CppSQLite3DB m_database;
	FilePath m_dbFilePath;

	StorageModeType m_mode;

private:
	virtual size_t getStaticStorageVersion() const = 0;
	virtual std::vector<std::pair<int, SqliteDatabaseIndex>> getIndices() const = 0;
	virtual void clearTables() = 0;
	virtual void setupTables() = 0;

	std::vector<std::pair<int, SqliteDatabaseIndex>> m_indices;
};

#endif // SQLITE_STORAGE_H
