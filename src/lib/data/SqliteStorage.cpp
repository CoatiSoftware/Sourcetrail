#include "data/SqliteStorage.h"

#include "data/graph/Node.h"
#include "data/location/TokenLocation.h"
#include "data/DefinitionType.h"
#include "data/parser/ParseLocation.h"
#include "data/SqliteIndex.h"
#include "utility/logging/logging.h"
#include "utility/text/TextAccess.h"
#include "utility/utility.h"
#include "utility/utilityString.h"
#include "utility/Version.h"

const size_t SqliteStorage::STORAGE_VERSION = 1;

SqliteStorage::SqliteStorage(const FilePath& dbFilePath)
	: m_dbFilePath(dbFilePath)
{
	m_database.open(m_dbFilePath.str().c_str());

	m_database.execDML("PRAGMA foreign_keys=ON;");
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
	m_database.execDML("PRAGMA foreign_keys=ON;");
	setupTables();
}

void SqliteStorage::clear()
{
	m_database.execDML("PRAGMA foreign_keys=OFF;");
	clearTables();

	setup();
}

void SqliteStorage::beginTransaction()
{
	m_database.execDML("BEGIN TRANSACTION;");
}

void SqliteStorage::commitTransaction()
{
	m_database.execDML("COMMIT TRANSACTION;");
}

void SqliteStorage::rollbackTransaction()
{
	m_database.execDML("ROLLBACK TRANSACTION;");
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

void SqliteStorage::setVersion()
{
	setStorageVersion();
	setApplicationVersion();
}

Id SqliteStorage::addEdge(int type, Id sourceNodeId, Id targetNodeId)
{
	m_database.execDML(
		"INSERT INTO element(id) VALUES(NULL);"
	);
	Id id = m_database.lastRowId();

	m_database.execDML((
		"INSERT INTO edge(id, type, source_node_id, target_node_id) VALUES("
		+ std::to_string(id) + ", " + std::to_string(type) + ", "
		+ std::to_string(sourceNodeId) + ", " + std::to_string(targetNodeId) + ");"
	).c_str());

	return id;
}

Id SqliteStorage::addNode(int type, const std::string& serializedName, int definitionType)
{
	m_database.execDML(
		"INSERT INTO element(id) VALUES(NULL);"
	);
	Id id = m_database.lastRowId();

	CppSQLite3Statement stmt = m_database.compileStatement((
		"INSERT INTO node(id, type, serialized_name, definition_type) VALUES("
		+ std::to_string(id) + ", " + std::to_string(type) + ", ?, " + std::to_string(definitionType) + ");"
	).c_str());

	stmt.bind(1, serializedName.c_str());
	stmt.execDML();

	return id;
}

Id SqliteStorage::addFile(const std::string& serializedName, const std::string& filePath, const std::string& modificationTime)
{
	Id id = addNode(Node::NODE_FILE, serializedName, definitionTypeToInt(DEFINITION_EXPLICIT));
	std::shared_ptr<TextAccess> content = TextAccess::createFromFile(filePath);
	unsigned int loc = content->getLineCount();

	CppSQLite3Statement stmt = m_database.compileStatement((
		"INSERT INTO file(id, path, modification_time, content, loc) VALUES("
		+ std::to_string(id) + ", '" + filePath + "', '" + modificationTime + "', ?, " + std::to_string(loc) + ");"
	).c_str());

	stmt.bind(1, content->getText().c_str());
	stmt.execDML();

	return id;
}

Id SqliteStorage::addLocalSymbol(const std::string& name)
{
	m_database.execDML(
		"INSERT INTO element(id) VALUES(NULL);"
	);
	Id id = m_database.lastRowId();

	CppSQLite3Statement stmt = m_database.compileStatement((
		"INSERT INTO local_symbol(id, name) VALUES("
		+ std::to_string(id) + ", ?);"
	).c_str());

	stmt.bind(1, name.c_str());
	stmt.execDML();

	return id;
}

Id SqliteStorage::addSourceLocation(
	Id elementId, Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, int type)
{
	m_database.execDML((
		"INSERT INTO source_location(id, element_id, file_node_id, start_line, start_column, end_line, end_column, type) "
		"VALUES(NULL, " + std::to_string(elementId) + ", " + std::to_string(fileNodeId) + ", "
		+ std::to_string(startLine) + ", " + std::to_string(startCol) + ", "
		+ std::to_string(endLine) + ", " + std::to_string(endCol) + ", " + std::to_string(type) + ");"
	).c_str());

	return m_database.lastRowId();
}

Id SqliteStorage::addComponentAccess(Id memberEdgeId, int type)
{
	m_database.execDML((
		"INSERT INTO component_access(id, edge_id, type) "
		"VALUES (NULL, " + std::to_string(memberEdgeId) + ", " + std::to_string(type) + ");"
	).c_str());

	return m_database.lastRowId();
}

Id SqliteStorage::addCommentLocation(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol)
{
	m_database.execDML((
		"INSERT INTO comment_location(id, file_node_id, start_line, start_column, end_line, end_column) "
		"VALUES(NULL, "  + std::to_string(fileNodeId) + ", "
		+ std::to_string(startLine) + ", " + std::to_string(startCol) + ", "
		+ std::to_string(endLine) + ", " + std::to_string(endCol) + ");"
	).c_str());

	return m_database.lastRowId();
}

Id SqliteStorage::addError(const std::string& message, bool fatal, bool indexed, const std::string& filePath, uint lineNumber, uint columnNumber)
{
	std::string sanitizedMessage = utility::replace((fatal ? "Fatal: " : "Error: ") + message, "'", "''");

	// check for duplicate
	CppSQLite3Statement stmt = m_database.compileStatement((
	//CppSQLite3Query q = m_database.execQuery((
		"SELECT * FROM error WHERE "
			"message == ? AND "
			"fatal == " + std::to_string(fatal) + " AND "
			"file_path == '" + filePath + "' AND "
			"line_number == " + std::to_string(lineNumber) + " AND "
			"column_number == " + std::to_string(columnNumber) + ";"
	).c_str());

	stmt.bind(1, sanitizedMessage.c_str());
	CppSQLite3Query q = stmt.execQuery();

	if (!q.eof())
	{
		return q.getIntField(0, -1);
	}

	stmt.finalize();

	stmt = m_database.compileStatement((
		"INSERT INTO error(message, fatal, indexed, file_path, line_number, column_number) "
		"VALUES (?, " + std::to_string(fatal) + ", " + std::to_string(indexed) + ", '" + filePath +
		"', " + std::to_string(lineNumber) + ", " + std::to_string(columnNumber) + ");"
	).c_str());

	stmt.bind(1, sanitizedMessage.c_str());
	stmt.execDML();

	return m_database.lastRowId();
}

void SqliteStorage::removeElement(Id id)
{
	std::vector<Id> ids;
	ids.push_back(id);
	removeElements(ids);
}

void SqliteStorage::removeElements(const std::vector<Id>& ids)
{
	m_database.execDML((
		"DELETE FROM element WHERE id IN (" + utility::join(utility::toStrings(ids), ',') + ");"
	).c_str());
}

void SqliteStorage::removeElementsWithLocationInFiles(const std::vector<Id>& fileIds)
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, element_id FROM source_location WHERE file_node_id IN (" + utility::join(utility::toStrings(fileIds), ',') + ");"
	).c_str());

	std::vector<Id> sourceLocationIds;
	std::vector<Id> elementIds;
	while (!q.eof())
	{
		sourceLocationIds.push_back(q.getIntField(0, 0));
		elementIds.push_back(q.getIntField(1, 0));
		q.nextRow();
	}

	m_database.execDML((
		"DELETE FROM source_location WHERE id IN (" + utility::join(utility::toStrings(sourceLocationIds), ',') + ");"
	).c_str());

	m_database.execDML((
		"DELETE FROM element WHERE "
			"element.id IN (" + utility::join(utility::toStrings(elementIds), ',') + ") " // skip all elements that dont have a matching id.
			"AND element.id NOT IN ("
				"SELECT source_location.element_id FROM source_location WHERE source_location.element_id == element.id LIMIT 1"
			");" // delete all elements that dont have a source location. This query is executed for each element that passed the first test.
	).c_str());
}

void SqliteStorage::removeErrorsInFiles(const std::vector<FilePath>& filePaths)
{
	m_database.execDML((
		"DELETE FROM error WHERE file_path IN ('" + utility::join(utility::toStrings(filePaths), "', '") + "');"
	).c_str());
}

bool SqliteStorage::isEdge(Id elementId) const
{
	int count = m_database.execScalar(("SELECT count(*) FROM edge WHERE id = " + std::to_string(elementId) + ";").c_str());
	return (count > 0);
}

bool SqliteStorage::isNode(Id elementId) const
{
	int count = m_database.execScalar(("SELECT count(*) FROM node WHERE id = " + std::to_string(elementId) + ";").c_str());
	return (count > 0);
}

bool SqliteStorage::isFile(Id elementId) const
{
	int count = m_database.execScalar(("SELECT count(*) FROM file WHERE id = " + std::to_string(elementId) + ";").c_str());
	return (count > 0);
}

StorageEdge SqliteStorage::getEdgeById(Id edgeId) const
{
	return getFirst<StorageEdge>("WHERE id == " + std::to_string(edgeId));
}

StorageEdge SqliteStorage::getEdgeBySourceTargetType(Id sourceId, Id targetId, int type) const
{
	return getFirst<StorageEdge>("WHERE "
		"source_node_id == " + std::to_string(sourceId) + " AND "
		"target_node_id == " + std::to_string(targetId) + " AND "
		"type == " + std::to_string(type)
	);
}

std::vector<StorageEdge> SqliteStorage::getEdgesByIds(const std::vector<Id>& edgeIds) const
{
	return getAll<StorageEdge>("WHERE id IN (" + utility::join(utility::toStrings(edgeIds), ',') + ")");
}

std::vector<StorageEdge> SqliteStorage::getEdgesBySourceId(Id sourceId) const
{
	return getAll<StorageEdge>("WHERE source_node_id == " + std::to_string(sourceId));
}

std::vector<StorageEdge> SqliteStorage::getEdgesBySourceIds(const std::vector<Id>& sourceIds) const
{
	return getAll<StorageEdge>("WHERE source_node_id IN (" + utility::join(utility::toStrings(sourceIds), ',') + ")");
}

std::vector<StorageEdge> SqliteStorage::getEdgesByTargetId(Id targetId) const
{
	return getAll<StorageEdge>("WHERE target_node_id == " + std::to_string(targetId));
}

std::vector<StorageEdge> SqliteStorage::getEdgesByTargetIds(const std::vector<Id>& targetIds) const
{
	return getAll<StorageEdge>("WHERE target_node_id IN (" + utility::join(utility::toStrings(targetIds), ',') + ")");
}

std::vector<StorageEdge> SqliteStorage::getEdgesBySourceOrTargetId(Id id) const
{
	return getAll<StorageEdge>("WHERE source_node_id == " + std::to_string(id) + " OR target_node_id == " + std::to_string(id));
}

std::vector<StorageEdge> SqliteStorage::getEdgesByType(int type) const
{
	return getAll<StorageEdge>("WHERE type == " + std::to_string(type));
}

std::vector<StorageEdge> SqliteStorage::getEdgesBySourceType(Id sourceId, int type) const
{
	return getAll<StorageEdge>("WHERE source_node_id == " + std::to_string(sourceId) + " AND type == " + std::to_string(type));
}

std::vector<StorageEdge> SqliteStorage::getEdgesByTargetType(Id targetId, int type) const
{
	return getAll<StorageEdge>("WHERE target_node_id == " + std::to_string(targetId) + " AND type == " + std::to_string(type));
}

void SqliteStorage::optimizeMemory() const
{
	try
	{
		m_database.execDML("VACUUM;");
	}
	catch(CppSQLite3Exception e)
	{
		LOG_ERROR(e.errorMessage());
	}
}

StorageNode SqliteStorage::getNodeById(Id id) const
{
	if (id != 0)
	{
		return getFirst<StorageNode>("WHERE id == " + std::to_string(id));
	}
	return StorageNode();
}

StorageNode SqliteStorage::getNodeBySerializedName(const std::string& serializedName) const
{
	CppSQLite3Statement stmt = m_database.compileStatement(
		"SELECT id, type, serialized_name, definition_type FROM node WHERE serialized_name == ? LIMIT 1;"
	);

	stmt.bind(1, serializedName.c_str());
	CppSQLite3Query q = stmt.execQuery();

	if (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const int type = q.getIntField(1, -1);
		const std::string serializedName = q.getStringField(2, "");
		const int definitionType = q.getIntField(3, 0);

		if (id != 0 && type != -1)
		{
			return StorageNode(id, type, serializedName, definitionType);
		}
	}

	return StorageNode();
}

std::vector<StorageNode> SqliteStorage::getNodesByIds(const std::vector<Id>& nodeIds) const
{
	return getAll<StorageNode>("WHERE id IN (" + utility::join(utility::toStrings(nodeIds), ',') + ")");
}

StorageLocalSymbol SqliteStorage::getLocalSymbolByName(const std::string& name) const
{
	return getFirst<StorageLocalSymbol>("WHERE name == '" + name + "'");
}

StorageFile SqliteStorage::getFileById(const Id id) const
{
	return getFirst<StorageFile>("WHERE node.id == " + std::to_string(id));
}

StorageFile SqliteStorage::getFileByPath(const FilePath& filePath) const
{
	return getFirst<StorageFile>("WHERE file.path == '" + filePath.str() + "'");
}

std::vector<StorageFile> SqliteStorage::getFilesByPaths(const std::vector<FilePath>& filePaths) const
{
	return getAll<StorageFile>("WHERE file.path IN ('" + utility::join(utility::toStrings(filePaths), "', '") + "')");
}

std::shared_ptr<TextAccess> SqliteStorage::getFileContentById(Id fileId) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT content FROM file WHERE id = '" + std::to_string(fileId) + "';"
	).c_str());

	if (!q.eof())
	{
		return TextAccess::createFromString(q.getStringField(0, ""));
	}

	return TextAccess::createFromString("");
}

std::shared_ptr<TextAccess> SqliteStorage::getFileContentByPath(const std::string& filePath) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT content FROM file WHERE path = '" + filePath + "';"
	).c_str());

	if (!q.eof())
	{
		return TextAccess::createFromString(q.getStringField(0, ""));
	}

	return TextAccess::createFromFile(filePath);
}

void SqliteStorage::setNodeType(int type, Id nodeId)
{
	m_database.execDML((
		"UPDATE node SET type = " + std::to_string(type) + " WHERE id == " + std::to_string(nodeId) + ";"
	).c_str());
}

void SqliteStorage::setNodeDefinitionType(int definitionType, Id nodeId)
{
	m_database.execDML((
		"UPDATE node SET definition_type = " + std::to_string(definitionType) + " WHERE id == " + std::to_string(nodeId) + ";"
	).c_str());
}

StorageSourceLocation SqliteStorage::getSourceLocationById(const Id id) const
{
	return getFirst<StorageSourceLocation>(
		"WHERE id == " + std::to_string(id) + ";"
	);
}

std::shared_ptr<TokenLocationFile> SqliteStorage::getTokenLocationsForFile(const FilePath& filePath) const
{
	std::shared_ptr<TokenLocationFile> ret = std::make_shared<TokenLocationFile>(filePath);

	const Id fileNodeId = getFileByPath(filePath.str()).id;
	if (fileNodeId == 0) // early out
	{
		return ret;
	}

	for (StorageSourceLocation& location: getAll<StorageSourceLocation>("WHERE file_node_id == " + std::to_string(fileNodeId)))
	{
		TokenLocation* loc = ret->addTokenLocation(
			location.id,
			location.elementId,
			location.startLine,
			location.startCol,
			location.endLine,
			location.endCol
		);
		loc->setType(intToLocationType(location.type));
	}

	ret->isWholeCopy = true;

	return ret;
}

std::vector<StorageSourceLocation> SqliteStorage::getTokenLocationsForElementId(const Id elementId) const
{
	std::vector<Id> elementIds {elementId};
	return getTokenLocationsForElementIds(elementIds);
}

std::vector<StorageSourceLocation> SqliteStorage::getTokenLocationsForElementIds(const std::vector<Id> elementIds) const
{
	return getAll<StorageSourceLocation>("WHERE element_id IN (" + utility::join(utility::toStrings(elementIds), ',') + ")");
}

Id SqliteStorage::getElementIdByLocationId(Id locationId) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT element_id FROM source_location WHERE id == " + std::to_string(locationId) + " LIMIT 1;"
	).c_str());
	if (!q.eof())
	{
		return q.getIntField(0, 0);
	}
	return 0;
}

StorageComponentAccess SqliteStorage::getComponentAccessByMemberEdgeId(Id memberEdgeId) const
{
	return getFirst<StorageComponentAccess>("WHERE edge_id == " + std::to_string(memberEdgeId));
}

std::vector<StorageComponentAccess> SqliteStorage::getComponentAccessByMemberEdgeIds(const std::vector<Id>& memberEdgeIds) const
{
	return getAll<StorageComponentAccess>("WHERE edge_id IN (" + utility::join(utility::toStrings(memberEdgeIds), ',') + ")");
}

std::vector<StorageCommentLocation> SqliteStorage::getCommentLocationsInFile(const FilePath& filePath) const
{
	Id fileNodeId = getFileByPath(filePath.str()).id;
	return getAll<StorageCommentLocation>("WHERE file_node_id == " + std::to_string(fileNodeId));
}

std::vector<StorageFile> SqliteStorage::getAllFiles() const
{
	return getAll<StorageFile>("");
}

std::vector<StorageNode> SqliteStorage::getAllNodes() const
{
	return getAll<StorageNode>("");
}

std::vector<StorageEdge> SqliteStorage::getAllEdges() const
{
	return getAll<StorageEdge>("");
}

std::vector<StorageLocalSymbol> SqliteStorage::getAllLocalSymbols() const
{
	return getAll<StorageLocalSymbol>("");
}

std::vector<StorageSourceLocation> SqliteStorage::getAllSourceLocations() const
{
	return getAll<StorageSourceLocation>("");
}

std::vector<StorageComponentAccess> SqliteStorage::getAllComponentAccesses() const
{
	return getAll<StorageComponentAccess>("");
}

std::vector<StorageCommentLocation> SqliteStorage::getAllCommentLocations() const
{
	return getAll<StorageCommentLocation>("");
}

std::vector<StorageError> SqliteStorage::getAllErrors() const
{
	return getAll<StorageError>("");
}

int SqliteStorage::getNodeCount() const
{
	return m_database.execScalar("SELECT COUNT(*) FROM node;");
}

int SqliteStorage::getEdgeCount() const
{
	return m_database.execScalar("SELECT COUNT(*) FROM edge;");
}

int SqliteStorage::getFileCount() const
{
	return m_database.execScalar("SELECT COUNT(*) FROM file;");
}

int SqliteStorage::getFileLOCCount() const
{
	return m_database.execScalar("SELECT SUM(loc) FROM file;");
}

int SqliteStorage::getSourceLocationCount() const
{
	return m_database.execScalar("SELECT COUNT(*) FROM source_location;");
}

void SqliteStorage::clearTables()
{
	m_database.execDML("DROP TABLE IF EXISTS main.error;");
	m_database.execDML("DROP TABLE IF EXISTS main.comment_location;");
	m_database.execDML("DROP TABLE IF EXISTS main.component_access;");
	m_database.execDML("DROP TABLE IF EXISTS main.source_location;");
	m_database.execDML("DROP TABLE IF EXISTS main.local_symbol;");
	m_database.execDML("DROP TABLE IF EXISTS main.file;");
	m_database.execDML("DROP TABLE IF EXISTS main.node;");
	m_database.execDML("DROP TABLE IF EXISTS main.edge;");
	m_database.execDML("DROP TABLE IF EXISTS main.element;");
	m_database.execDML("DROP TABLE IF EXISTS main.meta;");
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

		// TODO: move to createIndexesForAnalysis() or prepareForAnalysis
		m_database.execDML( // used for checking for duplicates during code analysis
			"CREATE INDEX IF NOT EXISTS edge_multipart_index ON edge(type, source_node_id, target_node_id);"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS node("
				"id INTEGER NOT NULL, "
				"type INTEGER NOT NULL, "
				"serialized_name TEXT, "
				"definition_type INTEGER NOT NULL, "
				"PRIMARY KEY(id), "
				"FOREIGN KEY(id) REFERENCES element(id) ON DELETE CASCADE);"
		);

		m_database.execDML(
			"CREATE INDEX IF NOT EXISTS node_serialized_name_index ON node(serialized_name);"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS file("
				"id INTEGER NOT NULL, "
				"path TEXT, "
				"modification_time TEXT, "
				"content TEXT, "
				"loc INTEGER, "
				"PRIMARY KEY(id), "
				"FOREIGN KEY(id) REFERENCES node(id) ON DELETE CASCADE);"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS local_symbol("
				"id INTEGER NOT NULL, "
				"name TEXT, "
				"PRIMARY KEY(id), "
				"FOREIGN KEY(id) REFERENCES element(id) ON DELETE CASCADE);"
		);

		m_database.execDML(
			"CREATE INDEX IF NOT EXISTS local_symbol_name_index ON local_symbol(name);"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS source_location("
				"id INTEGER NOT NULL, "
				"element_id INTEGER, "
				"file_node_id INTEGER, "
				"start_line INTEGER, "
				"start_column INTEGER, "
				"end_line INTEGER, "
				"end_column INTEGER, "
				"type INTEGER, "
				"PRIMARY KEY(id), "
				"FOREIGN KEY(element_id) REFERENCES element(id) ON DELETE CASCADE, "
				"FOREIGN KEY(file_node_id) REFERENCES node(id) ON DELETE CASCADE);"
		);

		SqliteIndex("source_location_element_id_index", "source_location(element_id)").createOnDatabase(m_database);
		SqliteIndex("source_location_file_node_id_index", "source_location(file_node_id)").createOnDatabase(m_database);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS component_access("
				"id INTEGER NOT NULL, "
				"edge_id INTEGER, "
				"type INTEGER NOT NULL, "
				"PRIMARY KEY(id), "
				"FOREIGN KEY(edge_id) REFERENCES edge(id) ON DELETE CASCADE);"
		);

		SqliteIndex("component_access_edge_id_index", "component_access(edge_id)").createOnDatabase(m_database);

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
	}
	catch (CppSQLite3Exception& e)
	{
		LOG_ERROR(std::to_string(e.errorCode()) + ": " + e.errorMessage());
	}
}

bool SqliteStorage::hasTable(const std::string& tableName) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT name FROM sqlite_master WHERE type='table' AND name='" + tableName + "';"
	).c_str());

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
		CppSQLite3Query q = m_database.execQuery(("SELECT value FROM meta WHERE key = '" + key + "';").c_str());

		if (!q.eof())
		{
			return q.getStringField(0, "");
		}
	}

	return "";
}

void SqliteStorage::insertOrUpdateMetaValue(const std::string& key, const std::string& value)
{
	m_database.execDML((
		"INSERT OR REPLACE INTO meta(id, key, value) "
			"VALUES( (SELECT id FROM meta WHERE key = '" + key + "'), '" + key + "', '" + value + "');"
	).c_str());
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
std::vector<StorageFile> SqliteStorage::getAll<StorageFile>(const std::string& query) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT file.id, node.serialized_name, file.path, file.modification_time FROM file "
			"INNER JOIN node ON file.id = node.id " + query + ";"
	).c_str());

	std::vector<StorageFile> files;
	while (!q.eof())
	{
		const Id id							= q.getIntField(0, 0);
		const std::string serializedName	= q.getStringField(1, "");
		const std::string filePath			= q.getStringField(2, "");
		const std::string modificationTime	= q.getStringField(3, "");

		if (id != 0)
		{
			files.push_back(StorageFile(id, serializedName, filePath, modificationTime));
		}
		q.nextRow();
	}

	return files;
}

template <>
std::vector<StorageEdge> SqliteStorage::getAll<StorageEdge>(const std::string& query) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, type, source_node_id, target_node_id FROM edge " + query + ";"
	).c_str());

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
std::vector<StorageNode> SqliteStorage::getAll<StorageNode>(const std::string& query) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, type, serialized_name, definition_type FROM node " + query + ";"
	).c_str());

	std::vector<StorageNode> nodes;
	while (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const int type = q.getIntField(1, -1);
		const std::string serializedName = q.getStringField(2, "");
		const int definitionType = q.getIntField(3, 0);

		if (id != 0 && type != -1)
		{
			nodes.push_back(StorageNode(id, type, serializedName, definitionType));
		}

		q.nextRow();
	}
	return nodes;
}

template <>
std::vector<StorageLocalSymbol> SqliteStorage::getAll<StorageLocalSymbol>(const std::string& query) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, name FROM local_symbol " + query + ";"
	).c_str());

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
std::vector<StorageSourceLocation> SqliteStorage::getAll<StorageSourceLocation>(const std::string& query) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, element_id, file_node_id, start_line, start_column, end_line, end_column, type FROM source_location " + query + ";"
	).c_str());

	std::vector<StorageSourceLocation> sourceLocations;

	while (!q.eof())
	{
		const Id id					= q.getIntField(0, 0);
		const Id elementId			= q.getIntField(1, 0);
		const Id fileNodeId			= q.getIntField(2, 0);
		const int startLineNumber	= q.getIntField(3, -1);
		const int startColNumber	= q.getIntField(4, -1);
		const int endLineNumber		= q.getIntField(5, -1);
		const int endColNumber		= q.getIntField(6, -1);
		const int type				= q.getIntField(7, -1);

		if (id != 0 && elementId != 0 && fileNodeId != 0 && startLineNumber != -1 && startColNumber != -1 && endLineNumber != -1 && endColNumber != -1 && type != -1)
		{
			sourceLocations.push_back(StorageSourceLocation(id, elementId, fileNodeId, startLineNumber, startColNumber, endLineNumber, endColNumber, type));
		}

		q.nextRow();
	}
	return sourceLocations;
}

template <>
std::vector<StorageComponentAccess> SqliteStorage::getAll<StorageComponentAccess>(const std::string& query) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, edge_id, type FROM component_access " + query + ";"
	).c_str());

	std::vector<StorageComponentAccess> componentAccesses;

	while (!q.eof())
	{
		const Id id		= q.getIntField(0, 0);
		const Id edgeId	= q.getIntField(1, 0);
		const int type	= q.getIntField(2, -1);

		if (id != 0 && edgeId != 0 && type != -1)
		{
			componentAccesses.push_back(StorageComponentAccess(edgeId, type));
		}

		q.nextRow();
	}
	return componentAccesses;
}

template <>
std::vector<StorageCommentLocation> SqliteStorage::getAll<StorageCommentLocation>(const std::string& query) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, file_node_id, start_line, start_column, end_line, end_column FROM comment_location " + query + ";"
	).c_str());

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
std::vector<StorageError> SqliteStorage::getAll<StorageError>(const std::string& query) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT message, fatal, indexed, file_path, line_number, column_number FROM error " + query + ";"
	).c_str());

	std::vector<StorageError> errors;
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
			errors.push_back(StorageError(message, fatal, indexed, filePath, lineNumber, columnNumber));
		}

		q.nextRow();
	}

	return errors;
}
