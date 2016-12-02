#include "data/SqliteStorage.h"

#include "data/graph/Node.h"
#include "data/location/TokenLocation.h"
#include "data/DefinitionType.h"
#include "data/parser/ParseLocation.h"
#include "utility/logging/logging.h"
#include "utility/text/TextAccess.h"
#include "utility/utility.h"
#include "utility/utilityString.h"
#include "utility/Version.h"

const size_t SqliteStorage::STORAGE_VERSION = 6;

SqliteStorage::SqliteStorage(const FilePath& dbFilePath)
	: m_dbFilePath(dbFilePath)
{
	m_database.open(m_dbFilePath.str().c_str());

	m_database.execDML("PRAGMA foreign_keys=ON;");

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
	m_database.execDML("PRAGMA foreign_keys=ON;");
	setupTables();
	m_mode = STORAGE_MODE_UNKNOWN;
}

void SqliteStorage::clear()
{
	m_database.execDML("PRAGMA foreign_keys=OFF;");
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

	m_database.execDML((
		"INSERT INTO file(id, path, modification_time, loc) VALUES("
		+ std::to_string(id) + ", '" + filePath + "', '" + modificationTime + "', " + std::to_string(loc) + ");"
	).c_str());

	CppSQLite3Statement stmt = m_database.compileStatement((
		"INSERT INTO filecontent(id, content) VALUES("
		+ std::to_string(id) + ", ?);"
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
	Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, int type)
{
	m_database.execDML((
		"INSERT INTO source_location(id, file_node_id, start_line, start_column, end_line, end_column, type) "
		"VALUES(NULL, " + std::to_string(fileNodeId) + ", "
		+ std::to_string(startLine) + ", " + std::to_string(startCol) + ", "
		+ std::to_string(endLine) + ", " + std::to_string(endCol) + ", " + std::to_string(type) + ");"
	).c_str());

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
	m_database.execDML((
		"INSERT INTO component_access(id, node_id, type) "
		"VALUES (NULL, " + std::to_string(nodeId) + ", " + std::to_string(type) + ");"
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

Id SqliteStorage::addError(const std::string& message, const FilePath& filePath, uint lineNumber, uint columnNumber, bool fatal, bool indexed)
{
	std::string sanitizedMessage = utility::replace(message, "'", "''");

	// check for duplicate
	CppSQLite3Statement stmt = m_database.compileStatement((
	//CppSQLite3Query q = m_database.execQuery((
		"SELECT * FROM error WHERE "
			"message == ? AND "
			"fatal == " + std::to_string(fatal) + " AND "
			"file_path == '" + filePath.str() + "' AND "
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
		"VALUES (?, " + std::to_string(fatal) + ", " + std::to_string(indexed) + ", '" + filePath.str() +
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
	// preparing
	m_database.execDML("DROP TABLE IF EXISTS main.element_id_to_clear;");

	m_database.execDML(
		"CREATE TABLE IF NOT EXISTS element_id_to_clear("
		"id INTEGER NOT NULL, "
		"PRIMARY KEY(id));"
	);

	// store ids of all elements located in fileIds into element_id_to_clear
	m_database.execDML((
		"INSERT INTO element_id_to_clear "
		"	SELECT occurrence.element_id "
		"	FROM occurrence "
		"	INNER JOIN source_location ON ("
		"		occurrence.source_location_id = source_location.id"
		"	) "
		"	WHERE source_location.file_node_id IN (" + utility::join(utility::toStrings(fileIds), ',') + ")"
		"	GROUP BY (occurrence.element_id)"
	).c_str());

	// delete all edges in element_id_to_clear
	m_database.execDML(
		"DELETE FROM element WHERE element.id IN (SELECT element_id_to_clear.id FROM element_id_to_clear INNER JOIN edge ON (element_id_to_clear.id = edge.id))"
	);

	// delete all edges originating from element_id_to_clear
	m_database.execDML(
		"DELETE FROM element WHERE element.id IN (SELECT id FROM edge WHERE source_node_id IN (SELECT id FROM element_id_to_clear))"
	);

	// remove all edges from element_id_to_clear (they have been cleared by now and we can disregard them)
	m_database.execDML(
		"DELETE FROM element_id_to_clear WHERE id IN ("
		"	SELECT id FROM edge"
		")"
	);

	// remove all files from element_id_to_clear (they will be cleared later)
	m_database.execDML(
		"DELETE FROM element_id_to_clear WHERE id IN ("
		"	SELECT id FROM file"
		")"
	);

	// delete source locations from fileIds (this also deletes the respective occurrences)
	m_database.execDML((
		"DELETE FROM source_location WHERE file_node_id IN (" + utility::join(utility::toStrings(fileIds), ',') + ");"
	).c_str());

	// remove all ids from element_id_to_clear that still have occurrences
	m_database.execDML(
		"DELETE FROM element_id_to_clear WHERE id IN ("
		"	SELECT element_id_to_clear.id FROM element_id_to_clear INNER JOIN occurrence ON element_id_to_clear.id = occurrence.element_id"
		")"
	);

	// remove all ids from element_id_to_clear that still have an edge pointing to them
	m_database.execDML(
		"DELETE FROM element_id_to_clear WHERE id IN ("
		"	SELECT target_node_id FROM edge"
		")"
	);

	// delete all elements that are still listed in element_id_to_clear
	m_database.execDML(
		"DELETE FROM element WHERE id IN ("
		"	SELECT id FROM element_id_to_clear"
		")"
	);

	// cleaning up
	m_database.execDML("DROP TABLE IF EXISTS main.element_id_to_clear;");

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

std::vector<StorageEdge> SqliteStorage::getEdgesByTargetType(const std::vector<Id>& targetIds, int type) const
{
	return getAll<StorageEdge>("WHERE target_node_id IN (" + utility::join(utility::toStrings(targetIds), ',') + ") AND type == " + std::to_string(type));
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
	try
	{
		CppSQLite3Query q = m_database.execQuery((
			"SELECT content FROM filecontent WHERE id = '" + std::to_string(fileId) + "';"
		).c_str());
		if (!q.eof())
		{
			return TextAccess::createFromString(q.getStringField(0, ""));
		}
	}
	catch (CppSQLite3Exception& e)
	{
		LOG_ERROR(std::to_string(e.errorCode()) + ": " + e.errorMessage());
	}

	return TextAccess::createFromString("");
}

std::shared_ptr<TextAccess> SqliteStorage::getFileContentByPath(const std::string& filePath) const
{
	try
	{
		CppSQLite3Query q = m_database.execQuery((
			"SELECT filecontent.content "
				"FROM filecontent "
				"INNER JOIN file ON filecontent.id = file.id "
				"WHERE file.path = '" + filePath + "';"
		).c_str());

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

StorageSourceLocation SqliteStorage::getSourceLocationByAll(const Id fileNodeId, const uint startLine, const uint startCol, const uint endLine, const uint endCol, const int type) const
{
	return getFirst<StorageSourceLocation>(
		"WHERE file_node_id == " + std::to_string(fileNodeId) +
		" AND start_line == " + std::to_string(startLine) +
		" AND start_column == " + std::to_string(startCol) +
		" AND end_line == " + std::to_string(endLine) +
		" AND end_column == " + std::to_string(endCol) +
		" AND type == " + std::to_string(type) + ";"
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

	for (std::pair<StorageSourceLocation, Id> e: getAllSourceLocationsAndElementIds("WHERE source_location.file_node_id == " + std::to_string(fileNodeId)))
	{
		TokenLocation* loc = ret->addTokenLocation(
			e.first.id,
			e.second,
			e.first.startLine,
			e.first.startCol,
			e.first.endLine,
			e.first.endCol
		);
		loc->setType(intToLocationType(e.first.type));
	}

	ret->isWholeCopy = true;

	return ret;
}

std::vector<StorageSourceLocation> SqliteStorage::getSourceLocationsForElementId(const Id elementId) const
{
	std::vector<Id> elementIds {elementId};
	std::vector<StorageSourceLocation> ret;
	for (std::pair<StorageSourceLocation, Id> e: getSourceLocationsAndElementIdsForElementIds(elementIds))
	{
		ret.push_back(e.first);
	}
	return ret;
}

std::vector<std::pair<StorageSourceLocation, Id>> SqliteStorage::getSourceLocationsAndElementIdsForElementIds(const std::vector<Id> elementIds) const
{
	return getAllSourceLocationsAndElementIds("WHERE occurrence.element_id IN (" + utility::join(utility::toStrings(elementIds), ',') + ")");
}

std::vector<std::pair<StorageSourceLocation, Id>> SqliteStorage::getAllSourceLocationsAndElementIds(const std::string& query) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT "
			"source_location.id, "
			"source_location.file_node_id, "
			"source_location.start_line, "
			"source_location.start_column, "
			"source_location.end_line, "
			"source_location.end_column, "
			"source_location.type, "
			"occurrence.element_id "
		"FROM source_location "
		"INNER JOIN occurrence ON occurrence.source_location_id = source_location.id " + query + ";"
	).c_str());

	std::vector<std::pair<StorageSourceLocation, Id>> ret;
	while (!q.eof())
	{
		const Id id				= q.getIntField(0, 0);
		const Id  fileNodeId	= q.getIntField(1, 0);
		const int startLine		= q.getIntField(2, -1);
		const int startColumn	= q.getIntField(3, -1);
		const int endLine		= q.getIntField(4, -1);
		const int endColumn		= q.getIntField(5, -1);
		const int type			= q.getIntField(6, -1);
		const Id elementId		= q.getIntField(7, 0);

		if (id != 0 && fileNodeId != 0 && startLine != -1 && startColumn != -1 && endLine != -1 && endColumn != -1 && type != -1 && elementId != 0)
		{
			ret.push_back(std::make_pair(
				StorageSourceLocation(
					id,
					fileNodeId,
					startLine,
					startColumn,
					endLine,
					endColumn,
					type),
				elementId
			));
		}
		q.nextRow();
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
	return getAll<StorageOccurrence>("WHERE source_location_id IN (" + utility::join(utility::toStrings(locationIds), ',') + ")");
}

StorageComponentAccess SqliteStorage::getComponentAccessByNodeId(Id nodeId) const
{
	return getFirst<StorageComponentAccess>("WHERE node_id == " + std::to_string(nodeId));
}

std::vector<StorageComponentAccess> SqliteStorage::getComponentAccessesByNodeIds(const std::vector<Id>& nodeIds) const
{
	return getAll<StorageComponentAccess>("WHERE node_id IN (" + utility::join(utility::toStrings(nodeIds), ',') + ")");
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

std::vector<StorageOccurrence> SqliteStorage::getAllOccurrences() const
{
	return getAll<StorageOccurrence>("");
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
	m_database.execDML("DROP TABLE IF EXISTS main.occurrence;");
	m_database.execDML("DROP TABLE IF EXISTS main.source_location;");
	m_database.execDML("DROP TABLE IF EXISTS main.local_symbol;");
	m_database.execDML("DROP TABLE IF EXISTS main.filecontent;");
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
			"CREATE TABLE IF NOT EXISTS file("
				"id INTEGER NOT NULL, "
				"path TEXT, "
				"modification_time TEXT, "
				"loc INTEGER, "
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
	CppSQLite3Statement stmt = m_database.compileStatement(std::string(
		"INSERT OR REPLACE INTO meta(id, key, value) VALUES("
			"(SELECT id FROM meta WHERE key = ?), ?, ?"
		");"
	).c_str());

	stmt.bind(1, key.c_str());
	stmt.bind(2, key.c_str());
	stmt.bind(3, value.c_str());
	stmt.execDML();
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
		"SELECT id, file_node_id, start_line, start_column, end_line, end_column, type FROM source_location " + query + ";"
		).c_str());

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
std::vector<StorageOccurrence> SqliteStorage::getAll<StorageOccurrence>(const std::string& query) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT element_id, source_location_id FROM occurrence " + query + ";"
		).c_str());

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
std::vector<StorageComponentAccess> SqliteStorage::getAll<StorageComponentAccess>(const std::string& query) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, node_id, type FROM component_access " + query + ";"
	).c_str());

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
