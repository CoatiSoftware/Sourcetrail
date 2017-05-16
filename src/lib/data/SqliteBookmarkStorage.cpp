#include "SqliteBookmarkStorage.h"

#include "utility/logging/logging.h"
#include "utility/utility.h"
#include "utility/utilityString.h"
#include "data/SqliteStorageMigrationLambda.h"
#include "data/SqliteStorageMigrator.h"

const size_t SqliteBookmarkStorage::s_storageVersion = 2;

SqliteBookmarkStorage::SqliteBookmarkStorage(const FilePath& dbFilePath)
	: SqliteStorage(dbFilePath)
{
}

SqliteBookmarkStorage::~SqliteBookmarkStorage()
{
}

size_t SqliteBookmarkStorage::getStaticVersion() const
{
	return s_storageVersion;
}

void SqliteBookmarkStorage::migrateIfNecessary()
{
	SqliteStorageMigrator migrator;

	migrator.addMigration(2, std::make_shared<SqliteStorageMigrationLambda>([](const SqliteStorageMigration* migration, SqliteStorage* storage){
		migration->executeStatementInStorage(storage, "UPDATE bookmarked_node SET serialized_node_name = '::\tm' || serialized_node_name");
		migration->executeStatementInStorage(storage, "UPDATE bookmarked_edge SET serialized_source_node_name = '::\tm' || serialized_source_node_name");
		migration->executeStatementInStorage(storage, "UPDATE bookmarked_edge SET serialized_target_node_name = '::\tm' || serialized_target_node_name");
	}));

	migrator.migrate(this, SqliteBookmarkStorage::s_storageVersion);
}

Id SqliteBookmarkStorage::addBookmarkCategory(const std::string& name)
{
	std::string statement = "INSERT INTO bookmark_category(id, name) "
		"VALUES (NULL, ?);";

	CppSQLite3Statement stmt = m_database.compileStatement(statement.c_str());
	stmt.bind(1, name.c_str());

	executeStatement(stmt);
	const Id id = m_database.lastRowId();

	return id;
}

Id SqliteBookmarkStorage::addBookmark(const std::string& name, const std::string& comment, const std::string& timestamp, const Id categoryId)
{
	std::string statement = "INSERT INTO bookmark(id, name, comment, timestamp, category_id) "
		"VALUES (NULL, ?, ?, ?, " + std::to_string(categoryId) + ");";

	try
	{
		CppSQLite3Statement stmt = m_database.compileStatement(statement.c_str());
		stmt.bind(1, name.c_str());
		stmt.bind(2, comment.c_str());
		stmt.bind(3, timestamp.c_str());
		executeStatement(stmt);

		const Id id = m_database.lastRowId();
		return id;
	}
	catch (CppSQLite3Exception e)
	{
		LOG_ERROR(std::to_string(e.errorCode()) + ": " + e.errorMessage());
	}
	return 0;
}

Id SqliteBookmarkStorage::addBookmarkedNode(const Id bookmarkId, const std::string& nodeName)
{
	executeStatement("INSERT INTO bookmarked_element(id, bookmark_id) VALUES(NULL, " + std::to_string(bookmarkId) + ");");
	Id id = m_database.lastRowId();

	std::string statement = "INSERT INTO bookmarked_node(id, serialized_node_name) "
		"VALUES (" + std::to_string(id) + ", ?);";
	CppSQLite3Statement stmt = m_database.compileStatement(statement.c_str());
	stmt.bind(1, nodeName.c_str());
	executeStatement(stmt);

	return id;
}

Id SqliteBookmarkStorage::addBookmarkedEdge(const Id bookmarkId, const std::string& sourceNodeName, const std::string& targetNodeName, const int edgeType, const bool sourceNodeActive)
{
	executeStatement("INSERT INTO bookmarked_element(id, bookmark_id) VALUES(NULL, " + std::to_string(bookmarkId) + ");");
	Id id = m_database.lastRowId();

	std::string statement = "INSERT INTO bookmarked_edge(id, serialized_source_node_name, serialized_target_node_name, edge_type, source_node_active) "
		"VALUES (" + std::to_string(id) + ", ?, ?, " + std::to_string(edgeType) + ", " + std::to_string(sourceNodeActive) + ");";
	CppSQLite3Statement stmt = m_database.compileStatement(statement.c_str());
	stmt.bind(1, sourceNodeName.c_str());
	stmt.bind(2, targetNodeName.c_str());
	executeStatement(stmt);

	return id;
}

std::vector<StorageBookmark> SqliteBookmarkStorage::getAllBookmarks() const
{
	return doGetAll<StorageBookmark>("");
}

void SqliteBookmarkStorage::removeBookmark(const Id id)
{
	executeStatement(
		"DELETE FROM bookmark WHERE id = (" + std::to_string(id) + ");"
	);
}

std::vector<StorageBookmarkedNode> SqliteBookmarkStorage::getAllBookmarkedNodes() const
{
	return doGetAll<StorageBookmarkedNode>("");
}

std::vector<StorageBookmarkedEdge> SqliteBookmarkStorage::getAllBookmarkedEdges() const
{
	return doGetAll<StorageBookmarkedEdge>("");
}

void SqliteBookmarkStorage::updateBookmark(const Id bookmarkId, const std::string& name, const std::string& comment, const Id categoryId)
{
	executeStatement("UPDATE bookmark SET name = '" + name + "' WHERE id == " + std::to_string(bookmarkId) + ";");
	executeStatement("UPDATE bookmark SET comment = '" + comment + "' WHERE id == " + std::to_string(bookmarkId) + ";");
	executeStatement("UPDATE bookmark SET category_id = " + std::to_string(categoryId) + " WHERE id == " + std::to_string(bookmarkId) + ";");
}

std::vector<StorageBookmarkCategory> SqliteBookmarkStorage::getAllBookmarkCategories() const
{
	return doGetAll<StorageBookmarkCategory>("");
}

StorageBookmarkCategory SqliteBookmarkStorage::getBookmarkCategoryByName(const std::string& name) const
{
	return doGetFirst<StorageBookmarkCategory>("WHERE name == '" + name + "'");
}

void SqliteBookmarkStorage::removeBookmarkCategory(Id id)
{
	executeStatement(
		"DELETE FROM bookmark_category WHERE id = (" + std::to_string(id) + ");"
	);
}

std::vector<std::pair<int, SqliteDatabaseIndex>> SqliteBookmarkStorage::getIndices() const
{
	return std::vector<std::pair<int, SqliteDatabaseIndex>>();
}

void SqliteBookmarkStorage::clearTables()
{
	try
	{
		m_database.execDML("DROP TABLE IF EXISTS main.bookmarked_edge;");
		m_database.execDML("DROP TABLE IF EXISTS main.bookmarked_node;");
		m_database.execDML("DROP TABLE IF EXISTS main.bookmarked_element;");
		m_database.execDML("DROP TABLE IF EXISTS main.bookmark;");
		m_database.execDML("DROP TABLE IF EXISTS main.bookmark_category;");
	}
	catch (CppSQLite3Exception& e)
	{
		LOG_ERROR(std::to_string(e.errorCode()) + ": " + e.errorMessage());
	}
}

void SqliteBookmarkStorage::setupTables()
{
	try
	{
		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS bookmark_category("
				"id INTEGER NOT NULL, "
				"name TEXT, "
				"PRIMARY KEY(id)"
			");"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS bookmark("
				"id INTEGER NOT NULL, "
				"name TEXT, "
				"comment TEXT, "
				"timestamp TEXT, "
				"category_id INTEGER, "
				"FOREIGN KEY(category_id) REFERENCES bookmark_category(id) ON DELETE CASCADE, "
				"PRIMARY KEY(id)"
			");"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS bookmarked_element("
				"id INTEGER NOT NULL, "
				"bookmark_id INTEGER NOT NULL, "
				"FOREIGN KEY(bookmark_id) REFERENCES bookmark(id) ON DELETE CASCADE, "
				"PRIMARY KEY(id)"
			");"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS bookmarked_node("
				"id INTEGER NOT NULL, "
				"serialized_node_name TEXT, "
				"FOREIGN KEY(id) REFERENCES bookmarked_element(id) ON DELETE CASCADE, "
				"PRIMARY KEY(id)"
			");"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS bookmarked_edge("
				"id INTEGER NOT NULL, "
				"serialized_source_node_name TEXT, "
				"serialized_target_node_name TEXT, "
				"edge_type INTEGER, "
				"source_node_active INTEGER, "
				"FOREIGN KEY(id) REFERENCES bookmarked_element(id) ON DELETE CASCADE, "
				"PRIMARY KEY(id)"
			");"
		);
	}
	catch (CppSQLite3Exception& e)
	{
		LOG_ERROR_STREAM(<< "Failed to create tables: " << std::to_string(e.errorCode()) << ": " << e.errorMessage());
		throw e;
	}
	catch (std::exception& e)
	{
		LOG_ERROR_STREAM(<< "Failed to create tables: " << e.what());
		throw e;
	}
}

template <>
std::vector<StorageBookmarkCategory> SqliteBookmarkStorage::doGetAll<StorageBookmarkCategory>(const std::string& query) const
{
	CppSQLite3Query q = executeQuery(
		"SELECT id, name FROM bookmark_category " + query + ";"
	);

	std::vector<StorageBookmarkCategory> categories;
	while (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const std::string name = q.getStringField(1, "");

		if (id != 0 && name != "")
		{
			categories.push_back(StorageBookmarkCategory(id, name));
		}

		q.nextRow();
	}
	return categories;
}

template <>
std::vector<StorageBookmark> SqliteBookmarkStorage::doGetAll<StorageBookmark>(const std::string& query) const
{
	CppSQLite3Query q = executeQuery(
		"SELECT id, name, comment, timestamp, category_id FROM bookmark " + query + ";"
	);

	std::vector<StorageBookmark> bookmarks;
	while (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const std::string name = q.getStringField(1, "");
		const std::string comment = q.getStringField(2, "");
		const std::string timestamp = q.getStringField(3, "");
		const Id categoryId = q.getIntField(4, 0);

		if (id != 0 && name != "" && timestamp != "")
		{
			bookmarks.push_back(StorageBookmark(id, name, comment, timestamp, categoryId));
		}

		q.nextRow();
	}
	return bookmarks;
}

template <>
std::vector<StorageBookmarkedNode> SqliteBookmarkStorage::doGetAll<StorageBookmarkedNode>(const std::string& query) const
{
	CppSQLite3Query q = executeQuery(
		"SELECT "
			"bookmarked_node.id, bookmarked_element.bookmark_id, bookmarked_node.serialized_node_name "
		"FROM bookmarked_node "
		"INNER JOIN "
			"bookmarked_element ON bookmarked_node.id = bookmarked_element.id " + query + ";"
	);

	std::vector<StorageBookmarkedNode> bookmarkedNodes;
	while (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const Id bookmarkId = q.getIntField(1, 0);
		const std::string serializedNodeName = q.getStringField(2, "");

		if (id != 0 && bookmarkId != 0 && serializedNodeName != "")
		{
			bookmarkedNodes.push_back(StorageBookmarkedNode(id, bookmarkId, serializedNodeName));
		}

		q.nextRow();
	}
	return bookmarkedNodes;
}

template <>
std::vector<StorageBookmarkedEdge> SqliteBookmarkStorage::doGetAll<StorageBookmarkedEdge>(const std::string& query) const
{
	CppSQLite3Query q = executeQuery(
		"SELECT "
			"bookmarked_edge.id, bookmarked_element.bookmark_id, bookmarked_edge.serialized_source_node_name, bookmarked_edge.serialized_target_node_name, bookmarked_edge.edge_type, bookmarked_edge.source_node_active "
		"FROM bookmarked_edge "
		"INNER JOIN "
			"bookmarked_element ON bookmarked_edge.id = bookmarked_element.id " + query + ";"
	);

	std::vector<StorageBookmarkedEdge> bookmarkedEdges;
	while (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const Id bookmarkId = q.getIntField(1, 0);
		const std::string serializedSourceNodeName = q.getStringField(2, "");
		const std::string serializedTargetNodeName = q.getStringField(3, "");
		const int edgeType = q.getIntField(4, -1);
		const int sourceNodeActive = q.getIntField(5, -1);

		if (id != 0 && bookmarkId != 0 && serializedSourceNodeName != "" && serializedTargetNodeName != "" && edgeType != -1 && sourceNodeActive != -1)
		{
			bookmarkedEdges.push_back(StorageBookmarkedEdge(id, bookmarkId, serializedSourceNodeName, serializedTargetNodeName, edgeType, sourceNodeActive));
		}

		q.nextRow();
	}
	return bookmarkedEdges;
}
