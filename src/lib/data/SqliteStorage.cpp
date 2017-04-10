#include "data/SqliteStorage.h"

#include <unordered_map>

#include "data/graph/Node.h"
#include "data/parser/ParseLocation.h"
#include "utility/logging/logging.h"
#include "utility/text/TextAccess.h"
#include "utility/Version.h"

const size_t SqliteStorage::STORAGE_VERSION = 9;

SqliteStorage::SqliteStorage(const FilePath& dbFilePath)
	: m_dbFilePath(dbFilePath.canonical())
{
	m_database.open(m_dbFilePath.str().c_str());

	executeStatement("PRAGMA foreign_keys=ON;");

	m_mode = STORAGE_MODE_UNKNOWN;

	m_indices.push_back(std::make_pair(
		STORAGE_MODE_WRITE,
		SqliteIndex("edge_multipart_index", "edge(type, source_node_id, target_node_id)")
	));
	m_indices.push_back(std::make_pair(
		STORAGE_MODE_WRITE | STORAGE_MODE_READ | STORAGE_MODE_CLEAR,
		SqliteIndex("node_serialized_name_index", "node(serialized_name)")
	));
	m_indices.push_back(std::make_pair(
		STORAGE_MODE_WRITE,
		SqliteIndex("local_symbol_name_index", "local_symbol(name)")
	));
	m_indices.push_back(std::make_pair(
		STORAGE_MODE_READ | STORAGE_MODE_CLEAR,
		SqliteIndex("source_location_file_node_id_index", "source_location(file_node_id)")
	));
	m_indices.push_back(std::make_pair(
		STORAGE_MODE_WRITE,
		SqliteIndex("source_location_all_data_index", "source_location(file_node_id, start_line, start_column, end_line, end_column, type)")
	));
	m_indices.push_back(std::make_pair(
		STORAGE_MODE_READ | STORAGE_MODE_CLEAR,
		SqliteIndex("occurrence_element_id_index", "occurrence(element_id)")
	));
	m_indices.push_back(std::make_pair(
		STORAGE_MODE_CLEAR,
		SqliteIndex("occurrence_source_location_id_index", "occurrence(source_location_id)")
	));
	m_indices.push_back(std::make_pair(
		STORAGE_MODE_WRITE | STORAGE_MODE_READ | STORAGE_MODE_CLEAR,
		SqliteIndex("component_access_node_id_index", "component_access(node_id)")
	));
}

SqliteStorage::~SqliteStorage()
{
	try
	{
		m_database.close();
	}
	catch(CppSQLite3Exception e)
	{
		LOG_ERROR(e.errorMessage());
	}
}

void SqliteStorage::setup()
{
	executeStatement("PRAGMA foreign_keys=ON;");
	setupTables();
	m_mode = STORAGE_MODE_UNKNOWN;
}

void SqliteStorage::clear()
{
	executeStatement("PRAGMA foreign_keys=OFF;");
	clearTables();

	setup();
}

void SqliteStorage::setMode(const StorageModeType mode)
{
	if (mode == m_mode)
	{
		return;
	}

	for (size_t i = 0; i < m_indices.size(); i++)
	{
		if (m_indices[i].first & mode)
		{
			m_indices[i].second.createOnDatabase(m_database);
		}
		else
		{
			m_indices[i].second.removeFromDatabase(m_database);
		}
	}

	m_mode = mode;
}

void SqliteStorage::beginTransaction()
{
	executeStatement("BEGIN TRANSACTION;");
}

void SqliteStorage::commitTransaction()
{
	executeStatement("COMMIT TRANSACTION;");
}

void SqliteStorage::rollbackTransaction()
{
	executeStatement("ROLLBACK TRANSACTION;");
}

void SqliteStorage::optimizeMemory() const
{
	executeStatement("VACUUM;");
}

FilePath SqliteStorage::getDbFilePath() const
{
	return m_dbFilePath;
}

bool SqliteStorage::isEmpty() const
{
	size_t storageVersion = getStorageVersion();
	if (storageVersion > 0)
	{
		return false;
	}

	Version applicationVersion = getApplicationVersion();
	return applicationVersion.isEmpty();
}

bool SqliteStorage::isIncompatible() const
{
	size_t storageVersion = getStorageVersion();
	if (storageVersion == 0 || storageVersion != STORAGE_VERSION)
	{
		return true;
	}

	return false;
}

std::string SqliteStorage::getProjectSettingsText() const
{
	return getMetaValue("project_settings");
}

void SqliteStorage::setProjectSettingsText(std::string text)
{
	insertOrUpdateMetaValue("project_settings", text);
}

void SqliteStorage::setVersion()
{
	setStorageVersion();
	setApplicationVersion();
}

Id SqliteStorage::addEdge(int type, Id sourceNodeId, Id targetNodeId)
{
	executeStatement("INSERT INTO element(id) VALUES(NULL);");
	Id id = m_database.lastRowId();

	executeStatement(
		"INSERT INTO edge(id, type, source_node_id, target_node_id) VALUES("
		+ std::to_string(id) + ", " + std::to_string(type) + ", "
		+ std::to_string(sourceNodeId) + ", " + std::to_string(targetNodeId) + ");"
	);

	return id;
}

Id SqliteStorage::addNode(const int type, const std::string& serializedName)
{
	executeStatement("INSERT INTO element(id) VALUES(NULL);");
	Id id = m_database.lastRowId();

	CppSQLite3Statement stmt = m_database.compileStatement((
		"INSERT INTO node(id, type, serialized_name) VALUES("
		+ std::to_string(id) + ", " + std::to_string(type) + ", ?);"
	).c_str());

	stmt.bind(1, serializedName.c_str());
	executeStatement(stmt);

	return id;
}

void SqliteStorage::addSymbol(const int id, const int definitionKind)
{
	executeStatement(
		"INSERT INTO symbol(id, definition_kind) VALUES("
		+ std::to_string(id) + ", " + std::to_string(definitionKind) + ");"
	);
}

void SqliteStorage::addFile(const int id, const std::string& filePath, const std::string& modificationTime)
{
	std::shared_ptr<TextAccess> content = TextAccess::createFromFile(filePath);
	unsigned int lineCount = content->getLineCount();

	executeStatement(
		"INSERT INTO file(id, path, modification_time, line_count) VALUES("
		+ std::to_string(id) + ", '" + filePath + "', '" + modificationTime + "', " + std::to_string(lineCount) + ");"
	);

	CppSQLite3Statement stmt = m_database.compileStatement((
		"INSERT INTO filecontent(id, content) VALUES("
		+ std::to_string(id) + ", ?);"
	).c_str());

	stmt.bind(1, content->getText().c_str());
	executeStatement(stmt);
}

Id SqliteStorage::addLocalSymbol(const std::string& name)
{
	executeStatement("INSERT INTO element(id) VALUES(NULL);");
	Id id = m_database.lastRowId();

	CppSQLite3Statement stmt = m_database.compileStatement((
		"INSERT INTO local_symbol(id, name) VALUES("
		+ std::to_string(id) + ", ?);"
	).c_str());

	stmt.bind(1, name.c_str());
	executeStatement(stmt);

	return id;
}

Id SqliteStorage::addSourceLocation(
	Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, int type)
{
	executeStatement(
		"INSERT INTO source_location(id, file_node_id, start_line, start_column, end_line, end_column, type) "
		"VALUES(NULL, " + std::to_string(fileNodeId) + ", "
		+ std::to_string(startLine) + ", " + std::to_string(startCol) + ", "
		+ std::to_string(endLine) + ", " + std::to_string(endCol) + ", " + std::to_string(type) + ");"
	);

	return m_database.lastRowId();
}

bool SqliteStorage::addOccurrence(Id elementId, Id sourceLocationId)
{
	try
	{
		m_database.execDML((
			"INSERT INTO occurrence(element_id, source_location_id) "
			"VALUES(" + std::to_string(elementId) + ", "
			+ std::to_string(sourceLocationId) + ");"
		).c_str());
	}
	catch (CppSQLite3Exception& e)
	{
		return false;
	}
	return true;
}

Id SqliteStorage::addComponentAccess(Id nodeId, int type)
{
	executeStatement(
		"INSERT INTO component_access(id, node_id, type) "
		"VALUES (NULL, " + std::to_string(nodeId) + ", " + std::to_string(type) + ");"
	);

	return m_database.lastRowId();
}

Id SqliteStorage::addCommentLocation(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol)
{
	executeStatement(
		"INSERT INTO comment_location(id, file_node_id, start_line, start_column, end_line, end_column) "
		"VALUES(NULL, "  + std::to_string(fileNodeId) + ", "
		+ std::to_string(startLine) + ", " + std::to_string(startCol) + ", "
		+ std::to_string(endLine) + ", " + std::to_string(endCol) + ");"
	);

	return m_database.lastRowId();
}

Id SqliteStorage::addError(const std::string& message, const FilePath& filePath, uint lineNumber, uint columnNumber, bool fatal, bool indexed)
{
	std::string sanitizedMessage = utility::replace(message, "'", "''");

	// check for duplicate
	CppSQLite3Statement stmt = m_database.compileStatement((
		"SELECT * FROM error WHERE "
			"message == ? AND "
			"fatal == " + std::to_string(fatal) + " AND "
			"file_path == '" + filePath.str() + "' AND "
			"line_number == " + std::to_string(lineNumber) + " AND "
			"column_number == " + std::to_string(columnNumber) + ";"
	).c_str());

	stmt.bind(1, sanitizedMessage.c_str());
	CppSQLite3Query q = executeQuery(stmt);

	if (!q.eof())
	{
		return q.getIntField(0, -1);
	}

	stmt.finalize();

	stmt = m_database.compileStatement((
		"INSERT INTO error(message, fatal, indexed, file_path, line_number, column_number) "
		"VALUES (?, " + std::to_string(fatal) + ", " + std::to_string(indexed) + ", '" + filePath.str() +
		"', " + std::to_string(lineNumber) + ", " + std::to_string(columnNumber) + ");"
	).c_str());

	stmt.bind(1, sanitizedMessage.c_str());
	executeStatement(stmt);

	return m_database.lastRowId();
}

Id SqliteStorage::addNodeBookmark(const NodeBookmark& bookmark)
{
	std::string tokenName = bookmark.getDisplayName();
	std::string comment = bookmark.getComment();
	std::string timeStamp = bookmark.getTimeStamp().toString();

	tokenName = utility::replace(tokenName, "'", "''");
	comment = utility::replace(comment, "'", "''");

	/*tokenName = utility::replace(tokenName, "\\", "/");
	comment = utility::replace(comment, "\\", "/");*/

	std::string statement = "INSERT INTO nodeBookmark(name, comment, timestamp, category) "
		"VALUES (?, ?, ?, ?);";

	int categoryId = getOrCreateBookmarkCategoryByName(bookmark.getCategory().getName()).getId();

	CppSQLite3Statement stmt = m_database.compileStatement(statement.c_str());
	stmt.bind(1, tokenName.c_str());
	stmt.bind(2, comment.c_str());
	stmt.bind(3, timeStamp.c_str());
	stmt.bind(4, categoryId);

	stmt.execDML();

	Id id = m_database.lastRowId();

	for (unsigned int i = 0; i < bookmark.getTokenNames().size(); i++)
	{
		std::string name = bookmark.getTokenNames()[i];
		int type = bookmark.getTokenTypes()[i];

		statement = "INSERT INTO nodeBookmarkToken(bookmarkId, name, type) "
			"VALUES (" + std::to_string(id) + ", ?, " + std::to_string(type) + ");";

		stmt = m_database.compileStatement(statement.c_str());
		stmt.bind(1, name.c_str());

		stmt.execDML();
	}

	return id;
}

Id SqliteStorage::addEdgeBookmark(const EdgeBookmark& bookmark)
{
	std::string tokenName = bookmark.getDisplayName();
	std::string comment = bookmark.getComment();
	std::string timeStamp = bookmark.getTimeStamp().toString();

	tokenName = utility::replace(tokenName, "'", "''");
	comment = utility::replace(comment, "'", "''");

	std::string statement = "INSERT INTO edgeBookmark(name, comment, timestamp, category) "
		"VALUES (?, ?, ?, ?);";

	int categoryId = getOrCreateBookmarkCategoryByName(bookmark.getCategory().getName()).getId();

	CppSQLite3Statement stmt = m_database.compileStatement(statement.c_str());
	stmt.bind(1, tokenName.c_str());
	stmt.bind(2, comment.c_str());
	stmt.bind(3, timeStamp.c_str());
	stmt.bind(4, categoryId);

	stmt.execDML();

	Id id = m_database.lastRowId();

	for (unsigned int i = 0; i < bookmark.getEdgeTokenNames().size(); i++)
	{
		std::string name = bookmark.getEdgeTokenNames()[i];
		int type = bookmark.getEdgeTokenTypes()[i];

		statement = "INSERT INTO edgeBookmarkToken(bookmarkId, name, type) "
			"VALUES (" + std::to_string(id) + ", ?, " + std::to_string(type) + ");";

		stmt = m_database.compileStatement(statement.c_str());
		stmt.bind(1, name.c_str());

		stmt.execDML();
	}

	statement = "INSERT INTO edgeBaseBookmark(edgeId) "
		"VALUES (" + std::to_string(id) + ");";
	stmt = m_database.compileStatement(statement.c_str());
	stmt.execDML();
	Id baseId = m_database.lastRowId();

	for (unsigned int i = 0; i < bookmark.getTokenNames().size(); i++)
	{
		statement = "INSERT INTO edgeBaseBookmarkToken(bookmarkId, name, type) "
			"VALUES (" + std::to_string(baseId) + ", ?, " + std::to_string(bookmark.getTokenTypes()[i]) + ");";

		stmt = m_database.compileStatement(statement.c_str());
		stmt.bind(1, bookmark.getTokenNames()[i].c_str());

		stmt.execDML();
	}

	return id;
}

Id SqliteStorage::addBookmarkCategory(const std::string& name)
{
	std::string statement = "INSERT INTO bookmarkCategory(name) "
		"VALUES (?);";

	CppSQLite3Statement stmt = m_database.compileStatement(statement.c_str());
	stmt.bind(1, name.c_str());

	stmt.execDML();

	Id id = m_database.lastRowId();

	return id;
}

void SqliteStorage::removeElement(Id id)
{
	std::vector<Id> ids;
	ids.push_back(id);
	removeElements(ids);
}

void SqliteStorage::removeElements(const std::vector<Id>& ids)
{
	executeStatement(
		"DELETE FROM element WHERE id IN (" + utility::join(utility::toStrings(ids), ',') + ");"
	);
}

void SqliteStorage::removeElementsWithLocationInFiles(const std::vector<Id>& fileIds, std::function<void(int)> updateStatusCallback)
{
	if (updateStatusCallback != nullptr)
	{
		updateStatusCallback(1);
	}

	// preparing
	executeStatement("DROP TABLE IF EXISTS main.element_id_to_clear;");

	if (updateStatusCallback != nullptr)
	{
		updateStatusCallback(2);
	}

	executeStatement(
		"CREATE TABLE IF NOT EXISTS element_id_to_clear("
		"id INTEGER NOT NULL, "
		"PRIMARY KEY(id));"
	);

	if (updateStatusCallback != nullptr)
	{
		updateStatusCallback(3);
	}

	// store ids of all elements located in fileIds into element_id_to_clear
	executeStatement(
		"INSERT INTO element_id_to_clear "
		"	SELECT occurrence.element_id "
		"	FROM occurrence "
		"	INNER JOIN source_location ON ("
		"		occurrence.source_location_id = source_location.id"
		"	) "
		"	WHERE source_location.file_node_id IN (" + utility::join(utility::toStrings(fileIds), ',') + ")"
		"	GROUP BY (occurrence.element_id)"
	);

	if (updateStatusCallback != nullptr)
	{
		updateStatusCallback(4);
	}

	// delete all edges in element_id_to_clear
	executeStatement(
		"DELETE FROM element WHERE element.id IN (SELECT element_id_to_clear.id FROM element_id_to_clear INNER JOIN edge ON (element_id_to_clear.id = edge.id))"
	);

	if (updateStatusCallback != nullptr)
	{
		updateStatusCallback(22);
	}

	// delete all edges originating from element_id_to_clear
	executeStatement(
		"DELETE FROM element WHERE element.id IN (SELECT id FROM edge WHERE source_node_id IN (SELECT id FROM element_id_to_clear))"
	);

	if (updateStatusCallback != nullptr)
	{
		updateStatusCallback(23);
	}

	// remove all edges from element_id_to_clear (they have been cleared by now and we can disregard them)
	executeStatement(
		"DELETE FROM element_id_to_clear WHERE id IN ("
		"	SELECT id FROM edge"
		")"
	);

	if (updateStatusCallback != nullptr)
	{
		updateStatusCallback(24);
	}

	// remove all files from element_id_to_clear (they will be cleared later)
	executeStatement(
		"DELETE FROM element_id_to_clear WHERE id IN ("
		"	SELECT id FROM file"
		")"
	);

	if (updateStatusCallback != nullptr)
	{
		updateStatusCallback(25);
	}

	// delete source locations from fileIds (this also deletes the respective occurrences)
	executeStatement(
		"DELETE FROM source_location WHERE file_node_id IN (" + utility::join(utility::toStrings(fileIds), ',') + ");"
	);

	if (updateStatusCallback != nullptr)
	{
		updateStatusCallback(34);
	}

	// remove all ids from element_id_to_clear that still have occurrences
	executeStatement(
		"DELETE FROM element_id_to_clear WHERE id IN ("
		"	SELECT element_id_to_clear.id FROM element_id_to_clear INNER JOIN occurrence ON element_id_to_clear.id = occurrence.element_id"
		")"
	);

	if (updateStatusCallback != nullptr)
	{
		updateStatusCallback(35);
	}

	// remove all ids from element_id_to_clear that still have an edge pointing to them
	executeStatement(
		"DELETE FROM element_id_to_clear WHERE id IN ("
		"	SELECT target_node_id FROM edge"
		")"
	);

	if (updateStatusCallback != nullptr)
	{
		updateStatusCallback(44);
	}

	// delete all elements that are still listed in element_id_to_clear
	executeStatement(
		"DELETE FROM element WHERE id IN ("
		"	SELECT id FROM element_id_to_clear"
		")"
	);

	if (updateStatusCallback != nullptr)
	{
		updateStatusCallback(80);
	}

	// cleaning up
	executeStatement("DROP TABLE IF EXISTS main.element_id_to_clear;");

	if (updateStatusCallback != nullptr)
	{
		updateStatusCallback(89);
	}
}

void SqliteStorage::removeErrorsInFiles(const std::vector<FilePath>& filePaths)
{
	executeStatement(
		"DELETE FROM error WHERE file_path IN ('" + utility::join(utility::toStrings(filePaths), "', '") + "');"
	);
}

bool SqliteStorage::isEdge(Id elementId) const
{
	int count = executeScalar("SELECT count(*) FROM edge WHERE id = " + std::to_string(elementId) + ";");
	return (count > 0);
}

bool SqliteStorage::isNode(Id elementId) const
{
	int count = executeScalar("SELECT count(*) FROM node WHERE id = " + std::to_string(elementId) + ";");
	return (count > 0);
}

bool SqliteStorage::isFile(Id elementId) const
{
	int count = executeScalar("SELECT count(*) FROM file WHERE id = " + std::to_string(elementId) + ";");
	return (count > 0);
}

StorageEdge SqliteStorage::getEdgeById(Id edgeId) const
{
	std::vector<StorageEdge> candidates = doGetAll<StorageEdge>("WHERE id = " + std::to_string(edgeId));

	if (candidates.size() > 0)
	{
		return candidates[0];
	}

	return StorageEdge();
}

StorageEdge SqliteStorage::getEdgeBySourceTargetType(Id sourceId, Id targetId, int type) const
{
	return doGetFirst<StorageEdge>("WHERE "
		"source_node_id == " + std::to_string(sourceId) + " AND "
		"target_node_id == " + std::to_string(targetId) + " AND "
		"type == " + std::to_string(type)
	);
}

std::vector<StorageEdge> SqliteStorage::getEdgesBySourceId(Id sourceId) const
{
	return doGetAll<StorageEdge>("WHERE source_node_id == " + std::to_string(sourceId));
}

std::vector<StorageEdge> SqliteStorage::getEdgesBySourceIds(const std::vector<Id>& sourceIds) const
{
	return doGetAll<StorageEdge>("WHERE source_node_id IN (" + utility::join(utility::toStrings(sourceIds), ',') + ")");
}

std::vector<StorageEdge> SqliteStorage::getEdgesByTargetId(Id targetId) const
{
	return doGetAll<StorageEdge>("WHERE target_node_id == " + std::to_string(targetId));
}

std::vector<StorageEdge> SqliteStorage::getEdgesByTargetIds(const std::vector<Id>& targetIds) const
{
	return doGetAll<StorageEdge>("WHERE target_node_id IN (" + utility::join(utility::toStrings(targetIds), ',') + ")");
}

std::vector<StorageEdge> SqliteStorage::getEdgesBySourceOrTargetId(Id id) const
{
	return doGetAll<StorageEdge>("WHERE source_node_id == " + std::to_string(id) + " OR target_node_id == " + std::to_string(id));
}

std::vector<StorageEdge> SqliteStorage::getEdgesByType(int type) const
{
	return doGetAll<StorageEdge>("WHERE type == " + std::to_string(type));
}

std::vector<StorageEdge> SqliteStorage::getEdgesBySourceType(Id sourceId, int type) const
{
	return doGetAll<StorageEdge>("WHERE source_node_id == " + std::to_string(sourceId) + " AND type == " + std::to_string(type));
}

std::vector<StorageEdge> SqliteStorage::getEdgesBySourcesType(const std::vector<Id>& sourceIds, int type) const
{
	return doGetAll<StorageEdge>("WHERE source_node_id IN (" + utility::join(utility::toStrings(sourceIds), ',') + ") AND type == " + std::to_string(type));
}

std::vector<StorageEdge> SqliteStorage::getEdgesByTargetType(Id targetId, int type) const
{
	return doGetAll<StorageEdge>("WHERE target_node_id == " + std::to_string(targetId) + " AND type == " + std::to_string(type));
}

std::vector<StorageEdge> SqliteStorage::getEdgesByTargetsType(const std::vector<Id>& targetIds, int type) const
{
	return doGetAll<StorageEdge>("WHERE target_node_id IN (" + utility::join(utility::toStrings(targetIds), ',') + ") AND type == " + std::to_string(type));
}

bool SqliteStorage::checkEdgeExists(Id edgeId) const
{
	CppSQLite3Statement stmt = m_database.compileStatement(
		("SELECT type FROM edge WHERE id == " + std::to_string(edgeId) + ";").c_str()
	);

	CppSQLite3Query q = executeQuery(stmt);

	if (!q.eof())
	{
		const int type = q.getIntField(0, -1);

		if (type != -1)
		{
			return true;
		}
	}

	return false;
}

StorageNode SqliteStorage::getNodeById(Id id) const
{
	std::vector<StorageNode> candidates = doGetAll<StorageNode>("WHERE id = " + std::to_string(id));

	if (candidates.size() > 0)
	{
		return candidates[0];
	}

	return StorageNode();
}

StorageNode SqliteStorage::getNodeBySerializedName(const std::string& serializedName) const
{
	CppSQLite3Statement stmt = m_database.compileStatement(
		"SELECT id, type, serialized_name FROM node WHERE serialized_name == ? LIMIT 1;"
	);

	stmt.bind(1, serializedName.c_str());
	CppSQLite3Query q = executeQuery(stmt);

	if (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const int type = q.getIntField(1, -1);
		const std::string serializedName = q.getStringField(2, "");

		if (id != 0 && type != -1)
		{
			return StorageNode(id, type, serializedName);
		}
	}

	return StorageNode();
}

bool SqliteStorage::checkNodeExistsByName(const std::string& serializedName) const
{
	CppSQLite3Statement stmt = m_database.compileStatement(
		"SELECT id FROM node WHERE serialized_name == ? LIMIT 1;"
	);

	stmt.bind(1, serializedName.c_str());
	CppSQLite3Query q = executeQuery(stmt);

	if (!q.eof())
	{
		const Id id = q.getIntField(0, 0);

		if (id != (Id)-1)
		{
			return true;
		}
	}

	return false;
}

StorageLocalSymbol SqliteStorage::getLocalSymbolByName(const std::string& name) const
{
	return doGetFirst<StorageLocalSymbol>("WHERE name == '" + name + "'");
}

StorageFile SqliteStorage::getFileByPath(const std::string& filePath) const
{
	return doGetFirst<StorageFile>("WHERE file.path == '" + filePath + "'");
}

std::vector<StorageFile> SqliteStorage::getFilesByPaths(const std::vector<FilePath>& filePaths) const
{
	return doGetAll<StorageFile>("WHERE file.path IN ('" + utility::join(utility::toStrings(filePaths), "', '") + "')");
}

std::shared_ptr<TextAccess> SqliteStorage::getFileContentById(Id fileId) const
{
	CppSQLite3Query q = executeQuery(
		"SELECT content FROM filecontent WHERE id = '" + std::to_string(fileId) + "';"
	);
	if (!q.eof())
	{
		return TextAccess::createFromString(q.getStringField(0, ""));
	}

	return TextAccess::createFromString("");
}

std::shared_ptr<TextAccess> SqliteStorage::getFileContentByPath(const std::string& filePath) const
{
	try
	{
		CppSQLite3Query q = executeQuery(
			"SELECT filecontent.content "
				"FROM filecontent "
				"INNER JOIN file ON filecontent.id = file.id "
				"WHERE file.path = '" + filePath + "';"
		);

		if (!q.eof())
		{
			return TextAccess::createFromString(q.getStringField(0, ""));
		}
	}
	catch (CppSQLite3Exception& e)
	{
		LOG_ERROR(std::to_string(e.errorCode()) + ": " + e.errorMessage());
	}

	return TextAccess::createFromFile(filePath);
}

void SqliteStorage::setNodeType(int type, Id nodeId)
{
	executeStatement(
		"UPDATE node SET type = " + std::to_string(type) + " WHERE id == " + std::to_string(nodeId) + ";"
	);
}

StorageSourceLocation SqliteStorage::getSourceLocationByAll(const Id fileNodeId, const uint startLine, const uint startCol, const uint endLine, const uint endCol, const int type) const
{
	return doGetFirst<StorageSourceLocation>(
		"WHERE file_node_id == " + std::to_string(fileNodeId) +
		" AND start_line == " + std::to_string(startLine) +
		" AND start_column == " + std::to_string(startCol) +
		" AND end_line == " + std::to_string(endLine) +
		" AND end_column == " + std::to_string(endCol) +
		" AND type == " + std::to_string(type) + ";"
	);
}

std::shared_ptr<SourceLocationFile> SqliteStorage::getSourceLocationsForFile(const FilePath& filePath) const
{
	std::shared_ptr<SourceLocationFile> ret = std::make_shared<SourceLocationFile>(filePath, true);

	const Id fileNodeId = getFileByPath(filePath.str()).id;
	if (fileNodeId == 0) // early out
	{
		return ret;
	}

	std::vector<Id> sourceLocationIds;
	std::unordered_map<Id, StorageSourceLocation> sourceLocationIdToData;
	for (const StorageSourceLocation& storageLocation: doGetAll<StorageSourceLocation>("WHERE file_node_id == " + std::to_string(fileNodeId)))
	{
		sourceLocationIds.push_back(storageLocation.id);
		sourceLocationIdToData[storageLocation.id] = storageLocation;
	}

	std::map<Id, std::vector<Id>> sourceLocationIdToElementIds;
	for (const StorageOccurrence& occurrence: getOccurrencesForLocationIds(sourceLocationIds))
	{
		sourceLocationIdToElementIds[occurrence.sourceLocationId].push_back(occurrence.elementId);
	}

	for (const std::pair<Id, std::vector<Id>>& p : sourceLocationIdToElementIds)
	{
		auto it = sourceLocationIdToData.find(p.first);
		if (it != sourceLocationIdToData.end())
		{
			ret->addSourceLocation(
				intToLocationType(it->second.type),
				it->second.id,
				p.second,
				it->second.startLine,
				it->second.startCol,
				it->second.endLine,
				it->second.endCol
			);
		}
	}

	return ret;
}

std::vector<StorageOccurrence> SqliteStorage::getOccurrencesForLocationId(Id locationId) const
{
	std::vector<Id> locationIds {locationId};
	return getOccurrencesForLocationIds(locationIds);
}

std::vector<StorageOccurrence> SqliteStorage::getOccurrencesForLocationIds(const std::vector<Id>& locationIds) const
{
	return doGetAll<StorageOccurrence>("WHERE source_location_id IN (" + utility::join(utility::toStrings(locationIds), ',') + ")");
}

std::vector<StorageOccurrence> SqliteStorage::getOccurrencesForElementIds(const std::vector<Id>& elementIds) const
{
	return doGetAll<StorageOccurrence>("WHERE element_id IN (" + utility::join(utility::toStrings(elementIds), ',') + ")");
}

StorageComponentAccess SqliteStorage::getComponentAccessByNodeId(Id nodeId) const
{
	return doGetFirst<StorageComponentAccess>("WHERE node_id == " + std::to_string(nodeId));
}

std::vector<StorageComponentAccess> SqliteStorage::getComponentAccessesByNodeIds(const std::vector<Id>& nodeIds) const
{
	return doGetAll<StorageComponentAccess>("WHERE node_id IN (" + utility::join(utility::toStrings(nodeIds), ',') + ")");
}

std::vector<StorageCommentLocation> SqliteStorage::getCommentLocationsInFile(const FilePath& filePath) const
{
	Id fileNodeId = getFileByPath(filePath.str()).id;
	return doGetAll<StorageCommentLocation>("WHERE file_node_id == " + std::to_string(fileNodeId));
}

std::vector<NodeBookmark> SqliteStorage::getAllNodeBookmarks() const
{
	return doGetAll<NodeBookmark>("");
}

NodeBookmark SqliteStorage::getNodeBookmarkById(const Id bookmarkId) const
{
	std::vector<NodeBookmark> candidates = doGetAll<NodeBookmark>("WHERE id = " + std::to_string(bookmarkId));

	if (candidates.size() > 0)
	{
		return candidates[0];
	}

	return NodeBookmark();
}

bool SqliteStorage::checkNodeBookmarkExistsByNames(const std::vector<std::string>& names) const
{
	// bookmarks can only be uniquly identified by their token names.
	// therefore, a bookmark exists if there is an exactly matching set of 'bookmarkToken's with a common foreign key

	if (names.size() <= 0)
	{
		return false;
	}

	std::set<int> bookmarkIds;
	bool insert = true;

	for (unsigned int i = 0; i < names.size(); i++)
	{
		CppSQLite3Query q = m_database.execQuery((
			"SELECT bookmarkId FROM nodeBookmarkToken WHERE name = '" + names[i] +"';"
			).c_str());

		bool contains = false;

		while (!q.eof())
		{
			int id = q.getIntField(0, -1);

			if (insert)
			{
				bookmarkIds.insert(id);
			}
			else
			{
				if (bookmarkIds.find(id) != bookmarkIds.end())
				{
					contains = true;
				}
			}

			q.nextRow();
		}

		if ((contains == false && insert == false)
			|| bookmarkIds.size() <= 0)
		{
			return false;
		}

		insert = false;
	}

	return true;
}

void SqliteStorage::removeNodeBookmark(Id id)
{
	executeStatement(
		"DELETE FROM nodeBookmark WHERE id = (" + std::to_string(id) + ");"
	);
}

void SqliteStorage::editNodeBookmark(const NodeBookmark& bookmark)
{
	std::string statement = "UPDATE nodeBookmark SET name=?, comment=?, category=? WHERE id=" + std::to_string(bookmark.getId()) + ";";

	BookmarkCategory category = getBookmarkCategoryByName(bookmark.getCategory().getName());
	if (category.getId() == (Id)-1)
	{
		category.setId(addBookmarkCategory(bookmark.getCategory().getName()));
		category.setName(bookmark.getCategory().getName());
	}

	CppSQLite3Statement stmt = m_database.compileStatement(statement.c_str());
	stmt.bind(1, bookmark.getDisplayName().c_str());
	stmt.bind(2, bookmark.getComment().c_str());
	stmt.bind(3, (int)category.getId());

	stmt.execDML();
}

std::vector<EdgeBookmark> SqliteStorage::getAllEdgeBookmarks() const
{
	return doGetAll<EdgeBookmark>("");
}

EdgeBookmark SqliteStorage::getEdgeBookmarkById(const Id bookmarkId) const
{
	std::vector<EdgeBookmark> candidates = doGetAll<EdgeBookmark>("WHERE id = " + std::to_string(bookmarkId));

	if (candidates.size() > 0)
	{
		return candidates[0];
	}

	return EdgeBookmark();
}

bool SqliteStorage::checkEdgeBookmarkExistsByNames(const std::vector<std::string>& names) const
{
	if (names.size() <= 0)
	{
		return false;
	}

	std::set<int> bookmarkIds;
	bool insert = true;

	for (unsigned int i = 0; i < names.size(); i++)
	{
		CppSQLite3Query q = m_database.execQuery((
			"SELECT bookmarkId FROM edgeBookmarkToken WHERE name = '" + names[i] + "';"
			).c_str());

		bool contains = false;

		while (!q.eof())
		{
			int id = q.getIntField(0, -1);

			if (insert)
			{
				bookmarkIds.insert(id);
			}
			else
			{
				if (bookmarkIds.find(id) != bookmarkIds.end())
				{
					contains = true;
				}
			}

			q.nextRow();
		}

		if ((contains == false && insert == false)
			|| bookmarkIds.size() <= 0)
		{
			return false;
		}

		insert = false;
	}

	return true;
}

void SqliteStorage::removeEdgeBookmark(Id id)
{
	executeStatement(
		"DELETE FROM edgeBookmark WHERE id = (" + std::to_string(id) + ");"
	);
}

void SqliteStorage::editEdgeBookmark(const EdgeBookmark& bookmark)
{
	std::string statement = "UPDATE edgeBookmark SET name=?, comment=?, category=? WHERE id=" + std::to_string(bookmark.getId()) + ";";

	BookmarkCategory category = getBookmarkCategoryByName(bookmark.getCategory().getName());
	if (category.getId() == (Id)-1)
	{
		category.setId(addBookmarkCategory(bookmark.getCategory().getName()));
		category.setName(bookmark.getCategory().getName());
	}

	CppSQLite3Statement stmt = m_database.compileStatement(statement.c_str());
	stmt.bind(1, bookmark.getDisplayName().c_str());
	stmt.bind(2, bookmark.getComment().c_str());
	stmt.bind(3, (int)category.getId());

	stmt.execDML();
}

std::vector<BookmarkCategory> SqliteStorage::getAllBookmarkCategories() const
{
	return doGetAll<BookmarkCategory>("");
}

BookmarkCategory SqliteStorage::getBookmarkCategoryByName(const std::string& name) const
{
	BookmarkCategory category;
	category.setName("");
	category.setId(-1);

	CppSQLite3Query q = m_database.execQuery((
		"SELECT id FROM bookmarkCategory WHERE name = '" + name + "';"
		).c_str());

	while (!q.eof())
	{
		int id = q.getIntField(0, -1);

		if (id > -1)
		{
			category.setName(name);
			category.setId(id);
		}

		q.nextRow();
	}

	return category;
}

BookmarkCategory SqliteStorage::getOrCreateBookmarkCategoryByName(const std::string& name)
{
	if (checkBookmarkCategoryExists(name))
	{
		return getBookmarkCategoryByName(name);
	}
	else
	{
		Id id = addBookmarkCategory(name);
		BookmarkCategory result;
		result.setName(name);
		result.setId(id);

		return result;
	}
}

bool SqliteStorage::checkBookmarkCategoryExists(const std::string& name) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT id FROM bookmarkCategory WHERE name = '" + name + "';"
		).c_str());

	while (!q.eof())
	{
		int id = q.getIntField(0, -1);

		if (id > -1)
		{
			return true;
		}

		q.nextRow();
	}

	return false;
}

void SqliteStorage::removeBookmarkCategory(Id id)
{
	executeStatement(
		"DELETE FROM bookmarkCategory WHERE id = (" + std::to_string(id) + ");"
	);
}

int SqliteStorage::getNodeCount() const
{
	return executeScalar("SELECT COUNT(*) FROM node;");
}

int SqliteStorage::getEdgeCount() const
{
	return executeScalar("SELECT COUNT(*) FROM edge;");
}

int SqliteStorage::getFileCount() const
{
	return executeScalar("SELECT COUNT(*) FROM file;");
}

int SqliteStorage::getFileLineSum() const
{
	return executeScalar("SELECT SUM(line_count) FROM file;");
}

int SqliteStorage::getSourceLocationCount() const
{
	return executeScalar("SELECT COUNT(*) FROM source_location;");
}

void SqliteStorage::clearTables()
{
	try
	{
		m_database.execDML("DROP TABLE IF EXISTS main.error;");
		m_database.execDML("DROP TABLE IF EXISTS main.comment_location;");
		m_database.execDML("DROP TABLE IF EXISTS main.component_access;");
		m_database.execDML("DROP TABLE IF EXISTS main.occurrence;");
		m_database.execDML("DROP TABLE IF EXISTS main.source_location;");
		m_database.execDML("DROP TABLE IF EXISTS main.local_symbol;");
		m_database.execDML("DROP TABLE IF EXISTS main.filecontent;");
		m_database.execDML("DROP TABLE IF EXISTS main.file;");
		m_database.execDML("DROP TABLE IF EXISTS main.symbol;");
		m_database.execDML("DROP TABLE IF EXISTS main.node;");
		m_database.execDML("DROP TABLE IF EXISTS main.edge;");
		m_database.execDML("DROP TABLE IF EXISTS main.element;");
		m_database.execDML("DROP TABLE IF EXISTS main.meta;");
	}
	catch (CppSQLite3Exception& e)
	{
		LOG_ERROR(std::to_string(e.errorCode()) + ": " + e.errorMessage());
	}
}

void SqliteStorage::setupTables()
{
	try
	{
		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS meta("
				"id INTEGER, "
				"key TEXT, "
				"value TEXT, "
				"PRIMARY KEY(id));"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS element("
				"id INTEGER, "
				"PRIMARY KEY(id));"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS edge("
				"id INTEGER NOT NULL, "
				"type INTEGER NOT NULL, "
				"source_node_id INTEGER NOT NULL, "
				"target_node_id INTEGER NOT NULL, "
				"PRIMARY KEY(id), "
				"FOREIGN KEY(id) REFERENCES element(id) ON DELETE CASCADE, "
				"FOREIGN KEY(source_node_id) REFERENCES node(id) ON DELETE CASCADE, "
				"FOREIGN KEY(target_node_id) REFERENCES node(id) ON DELETE CASCADE);"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS node("
				"id INTEGER NOT NULL, "
				"type INTEGER NOT NULL, "
				"serialized_name TEXT, "
				"PRIMARY KEY(id), "
				"FOREIGN KEY(id) REFERENCES element(id) ON DELETE CASCADE);"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS symbol("
				"id INTEGER NOT NULL, "
				"definition_kind INTEGER NOT NULL, "
				"PRIMARY KEY(id), "
				"FOREIGN KEY(id) REFERENCES node(id) ON DELETE CASCADE);"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS file("
				"id INTEGER NOT NULL, "
				"path TEXT, "
				"modification_time TEXT, "
				"line_count INTEGER, "
				"PRIMARY KEY(id), "
				"FOREIGN KEY(id) REFERENCES node(id) ON DELETE CASCADE);"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS filecontent("
				"id INTERGER, "
				"content TEXT, "
				"FOREIGN KEY(id)"
					"REFERENCES file(id)"
					"ON DELETE CASCADE "
					"ON UPDATE CASCADE);"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS local_symbol("
				"id INTEGER NOT NULL, "
				"name TEXT, "
				"PRIMARY KEY(id), "
				"FOREIGN KEY(id) REFERENCES element(id) ON DELETE CASCADE);"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS source_location("
				"id INTEGER NOT NULL, "
				"file_node_id INTEGER, "
				"start_line INTEGER, "
				"start_column INTEGER, "
				"end_line INTEGER, "
				"end_column INTEGER, "
				"type INTEGER, "
				"PRIMARY KEY(id), "
				"FOREIGN KEY(file_node_id) REFERENCES node(id) ON DELETE CASCADE);"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS occurrence(" // TODO: properly delete this on refresh
				"element_id INTEGER NOT NULL, "
				"source_location_id INTEGER NOT NULL, "
				"PRIMARY KEY(element_id, source_location_id), "
				"FOREIGN KEY(element_id) REFERENCES element(id) ON DELETE CASCADE, "
				"FOREIGN KEY(source_location_id) REFERENCES source_location(id) ON DELETE CASCADE);"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS component_access("
				"id INTEGER NOT NULL, "
				"node_id INTEGER, "
				"type INTEGER NOT NULL, "
				"PRIMARY KEY(id), "
				"FOREIGN KEY(node_id) REFERENCES node(id) ON DELETE CASCADE);"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS comment_location("
				"id INTEGER NOT NULL, "
				"file_node_id INTEGER, "
				"start_line INTEGER, "
				"start_column INTEGER, "
				"end_line INTEGER, "
				"end_column INTEGER, "
				"PRIMARY KEY(id), "
				"FOREIGN KEY(file_node_id) REFERENCES node(id) ON DELETE CASCADE);"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS error("
				"id INTEGER NOT NULL, "
				"message TEXT, "
				"fatal INTEGER NOT NULL, "
				"indexed INTEGER NOT NULL, "
				"file_path TEXT, "
				"line_number INTEGER, "
				"column_number INTEGER, "
				"PRIMARY KEY(id));"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS bookmarkCategory("
			"id INTEGER NOT NULL, "
			"name TEXT, "
			"PRIMARY KEY(id));"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS nodeBookmark("
			"id INTEGER NOT NULL, "
			"name TEXT, "
			"comment TEXT, "
			"timestamp TEXT, "
			"category INTEGER, "
			"PRIMARY KEY(id), "
			"FOREIGN KEY(category) REFERENCES bookmarkCategory(id));"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS edgeBookmark("
			"id INTEGER NOT NULL, "
			"name TEXT, "
			"comment TEXT, "
			"timestamp TEXT, "
			"category INTEGER, "
			"PRIMARY KEY(id), "
			"FOREIGN KEY(category) REFERENCES bookmarkCategory(id));"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS nodeBookmarkToken("
			"id INTEGER NOT NULL, "
			"bookmarkId INTEGER NOT NULL, "
			"name TEXT, "
			"type INTEGER, "
			"PRIMARY KEY(id), "
			"FOREIGN KEY(bookmarkId) REFERENCES nodeBookmark(id) ON DELETE CASCADE);"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS edgeBookmarkToken("
			"id INTEGER NOT NULL, "
			"bookmarkId INTEGER NOT NULL, "
			"name TEXT, "
			"type INTEGER, "
			"PRIMARY KEY(id), "
			"FOREIGN KEY(bookmarkId) REFERENCES edgeBookmark(id) ON DELETE CASCADE);"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS edgeBaseBookmark("
			"id INTEGER NOT NULL, "
			"edgeId INTEGER, "
			"PRIMARY KEY(id), "
			"FOREIGN KEY(edgeId) REFERENCES edgeBookmark(id) ON DELETE CASCADE);"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS edgeBaseBookmarkToken("
			"id INTEGER NOT NULL, "
			"bookmarkId INTEGER NOT NULL, "
			"name TEXT, "
			"type INTEGER, "
			"PRIMARY KEY(id), "
			"FOREIGN KEY(bookmarkId) REFERENCES edgeBaseBookmark(id) ON DELETE CASCADE);"
		);
	}
	catch (CppSQLite3Exception& e)
	{
		LOG_ERROR(std::to_string(e.errorCode()) + ": " + e.errorMessage());

		throw(std::exception());

		// todo: cancel project creation and destroy created files, display message
	}
}

void SqliteStorage::executeStatement(const std::string& statement) const
{
	try
	{
		m_database.execDML(statement.c_str());
	}
	catch(CppSQLite3Exception e)
	{
		LOG_ERROR(std::to_string(e.errorCode()) + ": " + e.errorMessage());
	}
}

void SqliteStorage::executeStatement(CppSQLite3Statement& statement) const
{
	try
	{
		statement.execDML();
	}
	catch(CppSQLite3Exception e)
	{
		LOG_ERROR(std::to_string(e.errorCode()) + ": " + e.errorMessage());
	}
}

int SqliteStorage::executeScalar(const std::string& statement) const
{
	int ret = 0;
	try
	{
		ret = m_database.execScalar(statement.c_str());
	}
	catch(CppSQLite3Exception e)
	{
		LOG_ERROR(std::to_string(e.errorCode()) + ": " + e.errorMessage());
	}
	return ret;
}

CppSQLite3Query SqliteStorage::executeQuery(const std::string& query) const
{
	try
	{
		return m_database.execQuery(query.c_str());
	}
	catch(CppSQLite3Exception e)
	{
		LOG_ERROR(std::to_string(e.errorCode()) + ": " + e.errorMessage());
	}
	return CppSQLite3Query();
}

CppSQLite3Query SqliteStorage::executeQuery(CppSQLite3Statement& statement) const
{
	try
	{
		return statement.execQuery();
	}
	catch(CppSQLite3Exception e)
	{
		LOG_ERROR(std::to_string(e.errorCode()) + ": " + e.errorMessage());
	}
	return CppSQLite3Query();
}

bool SqliteStorage::hasTable(const std::string& tableName) const
{
	CppSQLite3Query q = executeQuery(
		"SELECT name FROM sqlite_master WHERE type='table' AND name='" + tableName + "';"
	);

	if (!q.eof())
	{
		return q.getStringField(0, "") == tableName;
	}

	return false;
}

std::string SqliteStorage::getMetaValue(const std::string& key) const
{
	if (hasTable("meta"))
	{
		CppSQLite3Query q = executeQuery("SELECT value FROM meta WHERE key = '" + key + "';");

		if (!q.eof())
		{
			return q.getStringField(0, "");
		}
	}

	return "";
}

void SqliteStorage::insertOrUpdateMetaValue(const std::string& key, const std::string& value)
{
	CppSQLite3Statement stmt = m_database.compileStatement(std::string(
		"INSERT OR REPLACE INTO meta(id, key, value) VALUES("
			"(SELECT id FROM meta WHERE key = ?), ?, ?"
		");"
	).c_str());

	stmt.bind(1, key.c_str());
	stmt.bind(2, key.c_str());
	stmt.bind(3, value.c_str());
	executeStatement(stmt);
}

size_t SqliteStorage::getStorageVersion() const
{
	std::string storageVersionStr = getMetaValue("storage_version");

	if (storageVersionStr.size())
	{
		return std::stoi(storageVersionStr);
	}

	return 0;
}

void SqliteStorage::setStorageVersion()
{
	insertOrUpdateMetaValue("storage_version", std::to_string(STORAGE_VERSION));
}

Version SqliteStorage::getApplicationVersion() const
{
	std::string versionStr = getMetaValue("version");

	if (versionStr.size())
	{
		return Version::fromString(versionStr);
	}

	return Version();
}

void SqliteStorage::setApplicationVersion()
{
	insertOrUpdateMetaValue("version", Version::getApplicationVersion().toString());
}

template <>
std::vector<StorageEdge> SqliteStorage::doGetAll<StorageEdge>(const std::string& query) const
{
	CppSQLite3Query q = executeQuery(
		"SELECT id, type, source_node_id, target_node_id FROM edge " + query + ";"
	);

	std::vector<StorageEdge> edges;
	while (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const int type = q.getIntField(1, -1);
		const Id sourceId = q.getIntField(2, 0);
		const Id targetId = q.getIntField(3, 0);

		if (id != 0 && type != -1)
		{
			edges.push_back(StorageEdge(id, type, sourceId, targetId));
		}

		q.nextRow();
	}
	return edges;
}

template <>
std::vector<StorageNode> SqliteStorage::doGetAll<StorageNode>(const std::string& query) const
{
	CppSQLite3Query q = executeQuery(
		"SELECT id, type, serialized_name FROM node " + query + ";"
	);

	std::vector<StorageNode> nodes;
	while (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const int type = q.getIntField(1, -1);
		const std::string serializedName = q.getStringField(2, "");

		if (id != 0 && type != -1)
		{
			nodes.push_back(StorageNode(id, type, serializedName));
		}

		q.nextRow();
	}
	return nodes;
}

template <>
std::vector<StorageSymbol> SqliteStorage::doGetAll<StorageSymbol>(const std::string& query) const
{
	CppSQLite3Query q = executeQuery(
		"SELECT id, definition_kind FROM symbol " + query + ";"
	);

	std::vector<StorageSymbol> symbols;
	while (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const int definitionKind = q.getIntField(1, 0);

		if (id != 0)
		{
			symbols.push_back(StorageSymbol(id, definitionKind));
		}

		q.nextRow();
	}
	return symbols;
}

template <>
std::vector<StorageFile> SqliteStorage::doGetAll<StorageFile>(const std::string& query) const
{
	CppSQLite3Query q = executeQuery(
		"SELECT id, path, modification_time FROM file " + query + ";"
	);

	std::vector<StorageFile> files;
	while (!q.eof())
	{
		const Id id							= q.getIntField(0, 0);
		const std::string filePath			= q.getStringField(1, "");
		const std::string modificationTime	= q.getStringField(2, "");

		if (id != 0)
		{
			files.push_back(StorageFile(id, filePath, modificationTime));
		}
		q.nextRow();
	}

	return files;
}

template <>
std::vector<StorageLocalSymbol> SqliteStorage::doGetAll<StorageLocalSymbol>(const std::string& query) const
{
	CppSQLite3Query q = executeQuery(
		"SELECT id, name FROM local_symbol " + query + ";"
	);

	std::vector<StorageLocalSymbol> localSymbols;

	while (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const std::string name = q.getStringField(1, "");

		if (id != 0)
		{
			localSymbols.push_back(StorageLocalSymbol(id, name));
		}

		q.nextRow();
	}
	return localSymbols;
}

template <>
std::vector<StorageSourceLocation> SqliteStorage::doGetAll<StorageSourceLocation>(const std::string& query) const
{
	CppSQLite3Query q = executeQuery(
		"SELECT id, file_node_id, start_line, start_column, end_line, end_column, type FROM source_location " + query + ";"
	);

	std::vector<StorageSourceLocation> sourceLocations;

	while (!q.eof())
	{
		const Id id					= q.getIntField(0, 0);
		const Id fileNodeId			= q.getIntField(1, 0);
		const int startLineNumber	= q.getIntField(2, -1);
		const int startColNumber	= q.getIntField(3, -1);
		const int endLineNumber		= q.getIntField(4, -1);
		const int endColNumber		= q.getIntField(5, -1);
		const int type				= q.getIntField(6, -1);

		if (id != 0 && fileNodeId != 0 && startLineNumber != -1 && startColNumber != -1 && endLineNumber != -1 && endColNumber != -1 && type != -1)
		{
			sourceLocations.push_back(StorageSourceLocation(id, fileNodeId, startLineNumber, startColNumber, endLineNumber, endColNumber, type));
		}

		q.nextRow();
	}
	return sourceLocations;
}

template <>
std::vector<StorageOccurrence> SqliteStorage::doGetAll<StorageOccurrence>(const std::string& query) const
{
	CppSQLite3Query q = executeQuery(
		"SELECT element_id, source_location_id FROM occurrence " + query + ";"
	);

	std::vector<StorageOccurrence> occurrences;

	while (!q.eof())
	{
		const Id elementId			= q.getIntField(0, 0);
		const Id sourceLocationId	= q.getIntField(1, 0);

		if (elementId != 0 && sourceLocationId != 0)
		{
			occurrences.push_back(StorageOccurrence(elementId, sourceLocationId));
		}

		q.nextRow();
	}
	return occurrences;
}

template <>
std::vector<StorageComponentAccess> SqliteStorage::doGetAll<StorageComponentAccess>(const std::string& query) const
{
	CppSQLite3Query q = executeQuery(
		"SELECT id, node_id, type FROM component_access " + query + ";"
	);

	std::vector<StorageComponentAccess> componentAccesses;

	while (!q.eof())
	{
		const Id id		= q.getIntField(0, 0);
		const Id nodeId	= q.getIntField(1, 0);
		const int type	= q.getIntField(2, -1);

		if (id != 0 && nodeId != 0 && type != -1)
		{
			componentAccesses.push_back(StorageComponentAccess(nodeId, type));
		}

		q.nextRow();
	}
	return componentAccesses;
}

template <>
std::vector<StorageCommentLocation> SqliteStorage::doGetAll<StorageCommentLocation>(const std::string& query) const
{
	CppSQLite3Query q = executeQuery(
		"SELECT id, file_node_id, start_line, start_column, end_line, end_column FROM comment_location " + query + ";"
	);

	std::vector<StorageCommentLocation> commentLocations;

	while (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const Id fileNodeId = q.getIntField(1, 0);
		const int startLineNumber = q.getIntField(2, -1);
		const int startColNumber = q.getIntField(3, -1);
		const int endLineNumber = q.getIntField(4, -1);
		const int endColNumber = q.getIntField(5, -1);

		if (id != 0 && fileNodeId != 0 && startLineNumber != -1 && startColNumber != -1 && endLineNumber != -1 && endColNumber != -1)
		{
			commentLocations.push_back(StorageCommentLocation(
				id, fileNodeId, startLineNumber, startColNumber, endLineNumber, endColNumber
			));
		}

		q.nextRow();
	}
	return commentLocations;
}

template <>
std::vector<StorageError> SqliteStorage::doGetAll<StorageError>(const std::string& query) const
{
	CppSQLite3Query q = executeQuery(
		"SELECT message, fatal, indexed, file_path, line_number, column_number FROM error " + query + ";"
	);

	std::vector<StorageError> errors;
	Id id = 1;
	while (!q.eof())
	{
		const std::string message = q.getStringField(0, "");
		const bool fatal = q.getIntField(1, 0);
		const bool indexed = q.getIntField(2, 0);
		const std::string filePath = q.getStringField(3, "");
		const int lineNumber = q.getIntField(4, -1);
		const int columnNumber = q.getIntField(5, -1);

		if (lineNumber != -1 && columnNumber != -1)
		{
			errors.push_back(StorageError(id, message, filePath, lineNumber, columnNumber, fatal, indexed));
			id++;
		}

		q.nextRow();
	}

	return errors;
}

template <>
std::vector<NodeBookmark> SqliteStorage::doGetAll<NodeBookmark>(const std::string& query) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, name, comment, timestamp, category FROM nodeBookmark " + query + ";"
		).c_str());

	std::vector<NodeBookmark> bookmarks;
	bookmarks.clear();

	while (!q.eof())
	{
		const int id = q.getIntField(0, -1);
		const std::string name = q.getStringField(1, "");
		const std::string comment = q.getStringField(2, "");
		const std::string timeStamp = q.getStringField(3, "");
		const int categoryId = q.getIntField(4, -1);

		CppSQLite3Query qSub = m_database.execQuery((
			"SELECT name, type FROM nodeBookmarkToken WHERE bookmarkId = " + std::to_string(id)  + ";"
			).c_str());

		std::vector<std::string> tokenNames;
		std::vector<int> tokenTypes;

		while (!qSub.eof())
		{
			const std::string tokenName = qSub.getStringField(0, "");
			const int tokenType = qSub.getIntField(1, -1);
			tokenNames.push_back(tokenName);
			tokenTypes.push_back(tokenType);
			qSub.nextRow();
		}

		NodeBookmark bookmark(name, std::vector<Id>(), tokenNames, comment, TimePoint(timeStamp));
		bookmark.setId(id);
		bookmark.setTokenTypes(tokenTypes);

		qSub = m_database.execQuery((
			"SELECT id, name FROM bookmarkCategory WHERE id = " + std::to_string(categoryId) + ";"
			).c_str());

		while (!qSub.eof())
		{
			const int categoryId = qSub.getIntField(0, -1);
			const std::string name = qSub.getStringField(1, "");
			BookmarkCategory category;
			category.setId(categoryId);
			category.setName(name);
			bookmark.setCategory(category);

			qSub.nextRow();
		}

		bookmarks.push_back(bookmark);

		q.nextRow();
	}

	return bookmarks;
}

template <>
std::vector<EdgeBookmark> SqliteStorage::doGetAll<EdgeBookmark>(const std::string& query) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, name, comment, timestamp, category FROM edgeBookmark " + query + ";"
		).c_str());

	std::vector<EdgeBookmark> bookmarks;
	bookmarks.clear();

	while (!q.eof())
	{
		const int id = q.getIntField(0, -1);
		const std::string name = q.getStringField(1, "");
		const std::string comment = q.getStringField(2, "");
		const std::string timeStamp = q.getStringField(3, "");
		const int categoryId = q.getIntField(4, -1);

		CppSQLite3Query qSub = m_database.execQuery((
			"SELECT name, type FROM edgeBookmarkToken WHERE bookmarkId = " + std::to_string(id) + ";"
			).c_str());


		std::vector<std::string> tokenNames;
		std::vector<int> tokenTypes;
		while (!qSub.eof())
		{
			const std::string tokenName = qSub.getStringField(0, "");
			const int tokenType = qSub.getIntField(1, -1);
			tokenNames.push_back(tokenName);
			tokenTypes.push_back(tokenType);
			qSub.nextRow();
		}

		qSub = m_database.execQuery((
			"SELECT id FROM edgeBaseBookmark WHERE edgeId = " + std::to_string(id) + ";"
			).c_str());

		NodeBookmark baseBookmark; // there should only be one base bookmark, if there are more use the last one
		while (!qSub.eof())
		{
			const int baseId = qSub.getIntField(0, -1);

			baseBookmark.setId(baseId);

			CppSQLite3Query qSubSub = m_database.execQuery((
				"SELECT id, name, type FROM edgeBaseBookmarkToken WHERE bookmarkId = " + std::to_string(baseId) + ";"
				).c_str());

			std::vector<std::string> baseTokenNames;
			std::vector<int> baseTokenTypes;
			while (!qSubSub.eof())
			{
				const std::string baseTokenName = qSubSub.getStringField(1, "");
				const int baseTokenType = qSubSub.getIntField(2, -1);
				baseTokenNames.push_back(baseTokenName);
				baseTokenTypes.push_back(baseTokenType);
				qSubSub.nextRow();
			}

			baseBookmark.setTokenNames(baseTokenNames);
			baseBookmark.setTokenTypes(baseTokenTypes);

			qSub.nextRow();
		}

		EdgeBookmark bookmark(name, std::vector<Id>(), tokenNames, comment, TimePoint(timeStamp));
		bookmark.setTokenTypes(tokenTypes);
		bookmark.setBaseBookmark(baseBookmark);
		bookmark.setId(id);

		qSub = m_database.execQuery((
			"SELECT id, name FROM bookmarkCategory WHERE id = " + std::to_string(categoryId) + ";"
			).c_str());

		while (!qSub.eof())
		{
			const int categoryId = qSub.getIntField(0, -1);
			const std::string name = qSub.getStringField(1, "");
			BookmarkCategory category;
			category.setId(categoryId);
			category.setName(name);
			bookmark.setCategory(category);

			qSub.nextRow();
		}

		bookmarks.push_back(bookmark);

		q.nextRow();
	}

	return bookmarks;
}

template <>
std::vector<BookmarkCategory> SqliteStorage::doGetAll<BookmarkCategory>(const std::string& query) const
{
	std::vector<BookmarkCategory> categories;

	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, name FROM bookmarkCategory " + query + ";"
		).c_str());

	while (!q.eof())
	{
		const int id = q.getIntField(0, -1);
		const std::string name = q.getStringField(1, "");

		BookmarkCategory category;
		category.setId(id);
		category.setName(name);

		categories.push_back(category);

		q.nextRow();
	}

	return categories;
}
