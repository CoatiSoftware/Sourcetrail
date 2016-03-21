#include "data/SqliteStorage.h"

#include "data/graph/Node.h"
#include "data/location/TokenLocation.h"
#include "data/DefinitionType.h"
#include "data/SqliteIndex.h"
#include "utility/logging/logging.h"
#include "utility/text/TextAccess.h"
#include "utility/utility.h"
#include "utility/utilityString.h"
#include "utility/Version.h"

SqliteStorage::SqliteStorage(const std::string& dbFilePath)
{
	m_database.open(dbFilePath.c_str());

	m_database.execDML("PRAGMA foreign_keys=ON;");
}

SqliteStorage::~SqliteStorage()
{
	m_database.close();
}

bool SqliteStorage::init()
{
	Version version = getVersion();

	if (version.isEmpty())
	{
		setup();
		return false;
	}
	else if (version.isDifferentStorageVersionThan(Version::getApplicationVersion()))
	{
		clear();
		return false;
	}
	else
	{
		return true;
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

Version SqliteStorage::getVersion() const
{
	std::string versionStr = getMetaValue("version");

	if (versionStr.size())
	{
		return Version::fromString(versionStr);
	}

	return Version();
}

void SqliteStorage::setVersion(const Version& version)
{
	insertOrUpdateMetaValue("version", version.toString());
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

	m_database.execDML((
		"INSERT INTO node(id, type, serialized_name, definition_type) VALUES("
		+ std::to_string(id) + ", " + std::to_string(type) + ", '" + serializedName + "', " + std::to_string(definitionType) + ");"
	).c_str());

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

Id SqliteStorage::addSourceLocation(
	Id elementId, Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, bool isScope)
{
	m_database.execDML((
		"INSERT INTO source_location(id, element_id, file_node_id, start_line, start_column, end_line, end_column, is_scope) "
		"VALUES(NULL, " + std::to_string(elementId) + ", " + std::to_string(fileNodeId) + ", "
		+ std::to_string(startLine) + ", " + std::to_string(startCol) + ", "
		+ std::to_string(endLine) + ", " + std::to_string(endCol) + ", " + std::to_string(isScope) + ");"
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

Id SqliteStorage::addError(const std::string& message, bool fatal, const std::string& filePath, uint lineNumber, uint columnNumber)
{
	std::string sanitizedMessage = utility::replace((fatal ? "Fatal: " : "Error: ") + message, "'", "''");

	// check for duplicate
	CppSQLite3Query q = m_database.execQuery((
		"SELECT * FROM error WHERE "
			"message == '" + sanitizedMessage + "' AND "
			"fatal == " + std::to_string(fatal) + " AND "
			"file_path == '" + filePath + "' AND "
			"line_number == " + std::to_string(lineNumber) + " AND "
			"column_number == " + std::to_string(columnNumber) + ";"
	).c_str());

	if (!q.eof())
	{
		return q.getIntField(0, -1);
	}

	m_database.execDML((
		"INSERT INTO error(message, fatal, file_path, line_number, column_number) "
		"VALUES ('" + sanitizedMessage + "', " + std::to_string(fatal) + ", '" + filePath + "', " + std::to_string(lineNumber) + ", " + std::to_string(columnNumber) + ");"
	).c_str());

	return m_database.lastRowId();
}

void SqliteStorage::removeElement(Id id)
{
	m_database.execDML((
		"DELETE FROM element WHERE id == " + std::to_string(id) + ";"
	).c_str());
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

StorageNode SqliteStorage::getFirstNode() const
{
	std::vector<StorageNode> nodes = getAllNodes("LIMIT 1");
	if (nodes.size())
	{
		return nodes[0];
	}

	return StorageNode(0, 0, "", definitionTypeToInt(DEFINITION_NONE));
}

std::vector<StorageNode> SqliteStorage::getAllNodes() const
{
	return getAllNodes("");
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
	CppSQLite3Query q = m_database.execQuery((
		"SELECT type, source_node_id, target_node_id FROM edge WHERE "
			"id == " + std::to_string(edgeId) + ";"
	).c_str());

	if (!q.eof())
	{
		const int type = q.getIntField(0, -1);
		const Id sourceId = q.getIntField(1, 0);
		const Id targetId = q.getIntField(2, 0);

		if (type != -1 && sourceId != 0 && targetId != 0)
		{
			return StorageEdge(edgeId, type, sourceId, targetId);
		}
	}
	return StorageEdge(0, -1, 0, 0);
}

StorageEdge SqliteStorage::getEdgeBySourceTargetType(Id sourceId, Id targetId, int type) const
{
	StorageEdge edge(
		getFirstResult<Id>(
			"SELECT id FROM edge WHERE "
				"source_node_id == " + std::to_string(sourceId) + " AND "
				"target_node_id == " + std::to_string(targetId) + " AND "
				"type == " + std::to_string(type) + ";"
		),
		type, sourceId, targetId
	);
	return edge;
}

std::vector<StorageEdge> SqliteStorage::getEdgesByIds(const std::vector<Id>& edgeIds) const
{
	return getAllEdges("WHERE id IN (" + utility::join(utility::toStrings(edgeIds), ',') + ")");
}

std::vector<StorageEdge> SqliteStorage::getEdgesBySourceId(Id sourceId) const
{
	return getAllEdges("WHERE source_node_id == " + std::to_string(sourceId));
}

std::vector<StorageEdge> SqliteStorage::getEdgesBySourceIds(const std::vector<Id>& sourceIds) const
{
	return getAllEdges("WHERE source_node_id IN (" + utility::join(utility::toStrings(sourceIds), ',') + ")");
}

std::vector<StorageEdge> SqliteStorage::getEdgesByTargetId(Id targetId) const
{
	return getAllEdges("WHERE target_node_id == " + std::to_string(targetId));
}

std::vector<StorageEdge> SqliteStorage::getEdgesByTargetIds(const std::vector<Id>& targetIds) const
{
	return getAllEdges("WHERE target_node_id IN (" + utility::join(utility::toStrings(targetIds), ',') + ")");
}

std::vector<StorageEdge> SqliteStorage::getEdgesBySourceOrTargetId(Id id) const
{
	return getAllEdges("WHERE source_node_id == " + std::to_string(id) + " OR target_node_id == " + std::to_string(id));
}

std::vector<StorageEdge> SqliteStorage::getEdgesByType(int type) const
{
	return getAllEdges("WHERE type == " + std::to_string(type));
}

std::vector<StorageEdge> SqliteStorage::getEdgesBySourceType(Id sourceId, int type) const
{
	std::vector<StorageEdge> edges;

	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, target_node_id FROM edge WHERE "
			"source_node_id == " + std::to_string(sourceId) + " AND "
			"type == " + std::to_string(type) + ";"
	).c_str());

	while (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const Id targetId = q.getIntField(1, 0);

		if (id != 0 && targetId != 0)
		{
			edges.push_back(StorageEdge(id, type, sourceId, targetId));
		}

		q.nextRow();
	}
	return edges;
}

std::vector<StorageEdge> SqliteStorage::getEdgesByTargetType(Id targetId, int type) const
{
	std::vector<StorageEdge> edges;

	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, source_node_id FROM edge WHERE "
			"target_node_id == " + std::to_string(targetId) + " AND "
			"type == " + std::to_string(type) + ";"
	).c_str());

	while (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const Id sourceId = q.getIntField(1, 0);

		if (id != 0 && sourceId != 0)
		{
			edges.push_back(StorageEdge(id, type, sourceId, targetId));
		}

		q.nextRow();
	}
	return edges;
}

StorageNode SqliteStorage::getNodeById(Id id) const
{
	if (id != 0)
	{
		return getFirstNode("WHERE id == " + std::to_string(id));
	}
	return StorageNode(0, 0, 0, definitionTypeToInt(DEFINITION_NONE));
}

StorageNode SqliteStorage::getNodeBySerializedName(const std::string& serializedName) const
{
	return getFirstNode("WHERE serialized_name == '" + serializedName + "'");
}

std::vector<StorageNode> SqliteStorage::getNodesByIds(const std::vector<Id>& nodeIds) const
{
	return getAllNodes("WHERE id IN (" + utility::join(utility::toStrings(nodeIds), ',') + ")");
}

StorageFile SqliteStorage::getFileById(const Id id) const
{
	return getFirstFile(
		"SELECT node.id, node.serialized_name, file.path, file.modification_time FROM node INNER JOIN file ON node.id = file.id "
		"WHERE node.id == " + std::to_string(id) + ";"
	);
}

StorageFile SqliteStorage::getFileByPath(const std::string& filePath) const
{
	StorageFile storageFile = getFirstFile(
		"SELECT node.id, node.serialized_name, file.path, file.modification_time FROM node INNER JOIN file ON node.id = file.id "
		"WHERE file.path == '" + filePath + "';"
	);

	return storageFile;
}

std::vector<StorageFile> SqliteStorage::getAllFiles() const
{
	return getAllFiles("SELECT file.id, node.serialized_name, file.path, file.modification_time FROM file INNER JOIN node ON file.id = node.id;");
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
	return getFirstSourceLocation(
		"SELECT id, element_id, file_node_id, start_line, start_column, end_line, end_column, is_scope FROM source_location WHERE id == " + std::to_string(id) + ";"
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

	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, element_id, start_line, start_column, end_line, end_column, is_scope FROM source_location WHERE file_node_id == " + std::to_string(fileNodeId) + ";"
	).c_str());

	while (!q.eof())
	{
		const Id locationId			= q.getIntField(0, 0);
		const Id elementId			= q.getIntField(1, 0);
		const int startLineNumber	= q.getIntField(2, -1);
		const int startColNumber	= q.getIntField(3, -1);
		const int endLineNumber		= q.getIntField(4, -1);
		const int endColNumber		= q.getIntField(5, -1);
		const int isScope			= q.getIntField(6, -1);

		if (locationId != 0 && elementId != 0 && startLineNumber != -1 && startColNumber != -1 && endLineNumber != -1 && endColNumber != -1 && isScope != -1)
		{
			TokenLocation* loc = ret->addTokenLocation(locationId, elementId, startLineNumber, startColNumber, endLineNumber, endColNumber);
			loc->setType(isScope ? TokenLocation::LOCATION_SCOPE : TokenLocation::LOCATION_TOKEN);
		}
		q.nextRow();
	}

	return ret;
}

std::vector<StorageSourceLocation> SqliteStorage::getTokenLocationsForElementId(const Id elementId) const
{
	std::vector<Id> elementIds {elementId};
	return getTokenLocationsForElementIds(elementIds);
}

std::vector<StorageSourceLocation> SqliteStorage::getTokenLocationsForElementIds(const std::vector<Id> elementIds) const
{
	std::vector<StorageSourceLocation> locations;

	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, element_id, file_node_id, start_line, start_column, end_line, end_column, is_scope FROM source_location WHERE element_id IN (" + utility::join(utility::toStrings(elementIds), ',') + ");"
	).c_str());

	while (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const Id elementId = q.getIntField(1, 0);
		const Id fileNodeId = q.getIntField(2, 0);
		const int startLineNumber = q.getIntField(3, -1);
		const int startColNumber = q.getIntField(4, -1);
		const int endLineNumber = q.getIntField(5, -1);
		const int endColNumber = q.getIntField(6, -1);
		const int isScope = q.getIntField(7, -1);

		if (id != 0 && elementId != 0 && fileNodeId != 0 && startLineNumber != -1 && startColNumber != -1 && endLineNumber != -1 && endColNumber != -1 && isScope != -1)
		{
			locations.push_back(StorageSourceLocation(
				id, elementId, fileNodeId, startLineNumber, startColNumber, endLineNumber, endColNumber, isScope
			));
		}
		q.nextRow();
	}

	return locations;
}

Id SqliteStorage::getElementIdByLocationId(Id locationId) const
{
	return getFirstResult<Id>(
		"SELECT element_id FROM source_location WHERE id == " + std::to_string(locationId) + ";"
	);
}

StorageComponentAccess SqliteStorage::getComponentAccessByMemberEdgeId(Id memberEdgeId) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, edge_id, type FROM component_access WHERE edge_id == " + std::to_string(memberEdgeId) + ";"
	).c_str());

	while (!q.eof())
	{
		return StorageComponentAccess(
			q.getIntField(1, 0),
			q.getIntField(2, 0)
		);
	}

	return StorageComponentAccess(0, 0);
}

std::vector<StorageComponentAccess> SqliteStorage::getComponentAccessByMemberEdgeIds(const std::vector<Id>& memberEdgeIds) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, edge_id, type FROM component_access WHERE edge_id IN (" + utility::join(utility::toStrings(memberEdgeIds), ',') + ");"
	).c_str());

	std::vector<StorageComponentAccess> accesses;
	while (!q.eof())
	{
		accesses.push_back(StorageComponentAccess(
			q.getIntField(1, 0),
			q.getIntField(2, 0)
		));

		q.nextRow();
	}
	return accesses;
}

std::vector<StorageCommentLocation> SqliteStorage::getCommentLocationsInFile(const FilePath& filePath) const
{
	Id fileNodeId = getFileByPath(filePath.str()).id;
	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, file_node_id, start_line, start_column, end_line, end_column FROM comment_location "
		"WHERE file_node_id == " + std::to_string(fileNodeId) + ";"
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

std::vector<StorageError> SqliteStorage::getAllErrors() const
{
	CppSQLite3Query q = m_database.execQuery(
		"SELECT message, fatal, file_path, line_number, column_number FROM error;"
	);

	std::vector<StorageError> errors;
	while (!q.eof())
	{
		const std::string message = q.getStringField(0, "");
		const bool fatal = q.getIntField(1, 0);
		const std::string filePath = q.getStringField(2, "");
		const uint lineNumber = q.getIntField(3, 0);
		const uint columnNumber = q.getIntField(4, 0);

		errors.push_back(StorageError(message, fatal, filePath, lineNumber, columnNumber));

		q.nextRow();
	}

	return errors;
}

std::vector<StorageError> SqliteStorage::getFatalErrors() const
{
	CppSQLite3Query q = m_database.execQuery(
		"SELECT message, fatal, file_path, line_number, column_number FROM error WHERE fatal == 1;"
	);

	std::vector<StorageError> errors;
	while (!q.eof())
	{
		const std::string message = q.getStringField(0, "");
		const bool fatal = q.getIntField(1, 0);
		const std::string filePath = q.getStringField(2, "");
		const uint lineNumber = q.getIntField(3, 0);
		const uint columnNumber = q.getIntField(4, 0);

		errors.push_back(StorageError(message, fatal, filePath, lineNumber, columnNumber));

		q.nextRow();
	}

	return errors;
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
	m_database.execDML("DROP TABLE IF EXISTS main.file;");
	m_database.execDML("DROP TABLE IF EXISTS main.node;");
	m_database.execDML("DROP TABLE IF EXISTS main.edge;");
	m_database.execDML("DROP TABLE IF EXISTS main.element;");
	m_database.execDML("DROP TABLE IF EXISTS main.meta;");
}

void SqliteStorage::setupTables()
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

	m_database.execDML( // used for checking for duplicates during code analysis // TODO: move to createIndexesForAnalysis() or prepareForAnalysis
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
		"CREATE TABLE IF NOT EXISTS source_location("
			"id INTEGER NOT NULL, "
			"element_id INTEGER, "
			"file_node_id INTEGER, "
			"start_line INTEGER, "
			"start_column INTEGER, "
			"end_line INTEGER, "
			"end_column INTEGER, "
			"is_scope INTEGER, "
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
			"file_path TEXT, "
			"line_number INTEGER, "
			"column_number INTEGER, "
			"PRIMARY KEY(id));"
	);
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

StorageFile SqliteStorage::getFirstFile(const std::string& query) const
{
	CppSQLite3Query q = m_database.execQuery(query.c_str());

	if (!q.eof())
	{
		const Id id							= q.getIntField(0, 0);
		const std::string serializedName	= q.getStringField(1, "");
		const std::string filePath			= q.getStringField(2, "");
		const std::string modificationTime	= q.getStringField(3, "");

		if (id != 0)
		{
			return StorageFile(id, serializedName, filePath, modificationTime);
		}
	}
	return StorageFile(0, "", "", "");
}

std::vector<StorageFile> SqliteStorage::getAllFiles(const std::string& query) const
{
	std::vector<StorageFile> files;

	CppSQLite3Query q = m_database.execQuery(query.c_str());

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

StorageSourceLocation SqliteStorage::getFirstSourceLocation(const std::string& query) const
{
	CppSQLite3Query q = m_database.execQuery(query.c_str());

	if (!q.eof())
	{
		const Id id					= q.getIntField(0, 0);
		const Id elementId			= q.getIntField(1, 0);
		const Id fileNodeId			= q.getIntField(2, 0);
		const int startLineNumber	= q.getIntField(3, -1);
		const int startColNumber	= q.getIntField(4, -1);
		const int endLineNumber		= q.getIntField(5, -1);
		const int endColNumber		= q.getIntField(6, -1);
		const int isScope			= q.getIntField(7, -1);

		if (id != 0 && elementId != 0 && fileNodeId != 0 && startLineNumber != -1 && startColNumber != -1 && endLineNumber != -1 && endColNumber != -1 && isScope != -1)
		{
			return StorageSourceLocation(
				id, elementId, fileNodeId, startLineNumber, startColNumber, endLineNumber, endColNumber, isScope
			);
		}
	}

	return StorageSourceLocation(0, 0, 0, -1, -1, -1, -1, -1);
}

std::vector<StorageEdge> SqliteStorage::getAllEdges(const std::string& query) const
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

std::vector<StorageNode> SqliteStorage::getAllNodes(const std::string& query) const
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

StorageNode SqliteStorage::getFirstNode(const std::string& query) const
{
	std::vector<StorageNode> nodes = getAllNodes(query + " LIMIT 1");
	if (nodes.size() > 0)
	{
		return nodes[0];
	}
	return StorageNode(0, 0, "", definitionTypeToInt(DEFINITION_NONE));
}
