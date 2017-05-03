#include "data/SqliteIndexStorage.h"

#include <unordered_map>

#include "data/graph/Node.h"
#include "data/parser/ParseLocation.h"
#include "utility/logging/logging.h"
#include "utility/text/TextAccess.h"
#include "utility/Version.h"

const size_t SqliteIndexStorage::s_storageVersion = 11;

SqliteIndexStorage::SqliteIndexStorage(const FilePath& dbFilePath)
	: SqliteStorage(dbFilePath.canonical())
{
}

SqliteIndexStorage::~SqliteIndexStorage()
{
}

std::string SqliteIndexStorage::getProjectSettingsText() const
{
	return getMetaValue("project_settings");
}

void SqliteIndexStorage::setProjectSettingsText(std::string text)
{
	insertOrUpdateMetaValue("project_settings", text);
}

Id SqliteIndexStorage::addEdge(int type, Id sourceNodeId, Id targetNodeId)
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

Id SqliteIndexStorage::addNode(const int type, const std::string& serializedName)
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

void SqliteIndexStorage::addSymbol(const int id, const int definitionKind)
{
	executeStatement(
		"INSERT INTO symbol(id, definition_kind) VALUES("
		+ std::to_string(id) + ", " + std::to_string(definitionKind) + ");"
	);
}

void SqliteIndexStorage::addFile(const int id, const std::string& filePath, const std::string& modificationTime, bool complete)
{
	if (getFileByPath(filePath).id != 0)
	{
		return;
	}

	std::shared_ptr<TextAccess> content = TextAccess::createFromFile(filePath);
	const size_t lineCount = content->getLineCount();

	const bool success = executeStatement(
		"INSERT INTO file(id, path, modification_time, complete, line_count) VALUES("
		+ std::to_string(id) + ", '" + filePath + "', '" + modificationTime + "', '" + std::to_string(complete) + "', " + std::to_string(lineCount) + ");"
	);

	if (success)
	{
		CppSQLite3Statement stmt = m_database.compileStatement((
			"INSERT INTO filecontent(id, content) VALUES("
			+ std::to_string(id) + ", ?);"
		).c_str());

		stmt.bind(1, content->getText().c_str());
		executeStatement(stmt);
	}
}

Id SqliteIndexStorage::addLocalSymbol(const std::string& name)
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

Id SqliteIndexStorage::addSourceLocation(
	Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, int type)
{
	Id id = doGetFirst<StorageSourceLocation>(
		"WHERE "
			"file_node_id == " + std::to_string(fileNodeId) + " AND "
			"start_line == " + std::to_string(startLine) + " AND "
			"start_column == " + std::to_string(startCol) + " AND "
			"end_line == " + std::to_string(endLine) + " AND "
			"end_column == " + std::to_string(endCol) + " AND "
			"type == " + std::to_string(type)
	).id;

	if (id == 0)
	{
		const bool success = executeStatement(
			"INSERT INTO source_location(id, file_node_id, start_line, start_column, end_line, end_column, type) "
			"VALUES(NULL, " + std::to_string(fileNodeId) + ", "
			+ std::to_string(startLine) + ", " + std::to_string(startCol) + ", "
			+ std::to_string(endLine) + ", " + std::to_string(endCol) + ", " + std::to_string(type) + ");"
		);

		if (success)
		{
			id = m_database.lastRowId();
		}
	}

	return id;
}

bool SqliteIndexStorage::addOccurrence(Id elementId, Id sourceLocationId)
{
	if (doGetFirst<StorageOccurrence>(
			"WHERE "
			"element_id == " + std::to_string(elementId) + " AND "
			"source_location_id == " + std::to_string(sourceLocationId)
		).elementId != 0)
	{
		return false;
	}

	const bool success = executeStatement(
		"INSERT INTO occurrence(element_id, source_location_id) "
			"VALUES(" + std::to_string(elementId) + ", "
			+ std::to_string(sourceLocationId) + ");"
	);

	return success;
}

Id SqliteIndexStorage::addComponentAccess(Id nodeId, int type)
{
	Id id = getComponentAccessByNodeId(nodeId).id;

	if (id == 0)
	{
		const bool success = executeStatement(
			"INSERT INTO component_access(id, node_id, type) "
			"VALUES (NULL, " + std::to_string(nodeId) + ", " + std::to_string(type) + ");"
		);

		if (success)
		{
			id = m_database.lastRowId();
		}
	}

	return id;
}

Id SqliteIndexStorage::addCommentLocation(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol)
{
	Id id = doGetFirst<StorageCommentLocation>(
		"WHERE "
			"file_node_id == " + std::to_string(fileNodeId) + " AND "
			"start_line == " + std::to_string(startLine) + " AND "
			"start_column == " + std::to_string(startCol) + " AND "
			"end_line == " + std::to_string(endLine) + " AND "
			"end_column == " + std::to_string(endCol)
	).id;

	if (id == 0)
	{
		const bool success = executeStatement(
			"INSERT INTO comment_location(id, file_node_id, start_line, start_column, end_line, end_column) "
			"VALUES(NULL, "  + std::to_string(fileNodeId) + ", "
			+ std::to_string(startLine) + ", " + std::to_string(startCol) + ", "
			+ std::to_string(endLine) + ", " + std::to_string(endCol) + ");"
		);

		if (success)
		{
			id = m_database.lastRowId();
		}
	}

	return id;
}

Id SqliteIndexStorage::addError(const std::string& message, const FilePath& filePath, uint lineNumber, uint columnNumber, bool fatal, bool indexed)
{
	const std::string sanitizedMessage = utility::replace(message, "'", "''");

	CppSQLite3Statement selectStmt = m_database.compileStatement((
		"SELECT id FROM error WHERE "
			"message == ? AND "
			"fatal == " + std::to_string(fatal) + " AND "
			"file_path == '" + filePath.str() + "' AND "
			"line_number == " + std::to_string(lineNumber) + " AND "
			"column_number == " + std::to_string(columnNumber) + ";"
	).c_str());

	selectStmt.bind(1, sanitizedMessage.c_str());
	CppSQLite3Query q = executeQuery(selectStmt);

	Id id = 0;
	if (!q.eof())
	{
		id = q.getIntField(0, -1);
	}

	if (id == 0)
	{
		CppSQLite3Statement stmt = m_database.compileStatement((
			"INSERT INTO error(message, fatal, indexed, file_path, line_number, column_number) "
			"VALUES (?, " + std::to_string(fatal) + ", " + std::to_string(indexed) + ", '" + filePath.str() +
			"', " + std::to_string(lineNumber) + ", " + std::to_string(columnNumber) + ");"
		).c_str());

		stmt.bind(1, sanitizedMessage.c_str());
		const bool success = executeStatement(stmt);

		if (success)
		{
			id = m_database.lastRowId();
		}
	}

	return id;
}

void SqliteIndexStorage::removeElement(Id id)
{
	std::vector<Id> ids;
	ids.push_back(id);
	removeElements(ids);
}

void SqliteIndexStorage::removeElements(const std::vector<Id>& ids)
{
	executeStatement(
		"DELETE FROM element WHERE id IN (" + utility::join(utility::toStrings(ids), ',') + ");"
	);
}

void SqliteIndexStorage::removeElementsWithLocationInFiles(const std::vector<Id>& fileIds, std::function<void(int)> updateStatusCallback)
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

void SqliteIndexStorage::removeErrorsInFiles(const std::vector<FilePath>& filePaths)
{
	executeStatement(
		"DELETE FROM error WHERE file_path IN ('" + utility::join(utility::toStrings(filePaths), "', '") + "');"
	);
}

bool SqliteIndexStorage::isEdge(Id elementId) const
{
	int count = executeStatementScalar("SELECT count(*) FROM edge WHERE id = " + std::to_string(elementId) + ";");
	return (count > 0);
}

bool SqliteIndexStorage::isNode(Id elementId) const
{
	int count = executeStatementScalar("SELECT count(*) FROM node WHERE id = " + std::to_string(elementId) + ";");
	return (count > 0);
}

bool SqliteIndexStorage::isFile(Id elementId) const
{
	int count = executeStatementScalar("SELECT count(*) FROM file WHERE id = " + std::to_string(elementId) + ";");
	return (count > 0);
}

StorageEdge SqliteIndexStorage::getEdgeById(Id edgeId) const
{
	std::vector<StorageEdge> candidates = doGetAll<StorageEdge>("WHERE id = " + std::to_string(edgeId));

	if (candidates.size() > 0)
	{
		return candidates[0];
	}

	return StorageEdge();
}

StorageEdge SqliteIndexStorage::getEdgeBySourceTargetType(Id sourceId, Id targetId, int type) const
{
	return doGetFirst<StorageEdge>("WHERE "
		"source_node_id == " + std::to_string(sourceId) + " AND "
		"target_node_id == " + std::to_string(targetId) + " AND "
		"type == " + std::to_string(type)
	);
}

std::vector<StorageEdge> SqliteIndexStorage::getEdgesBySourceId(Id sourceId) const
{
	return doGetAll<StorageEdge>("WHERE source_node_id == " + std::to_string(sourceId));
}

std::vector<StorageEdge> SqliteIndexStorage::getEdgesBySourceIds(const std::vector<Id>& sourceIds) const
{
	return doGetAll<StorageEdge>("WHERE source_node_id IN (" + utility::join(utility::toStrings(sourceIds), ',') + ")");
}

std::vector<StorageEdge> SqliteIndexStorage::getEdgesByTargetId(Id targetId) const
{
	return doGetAll<StorageEdge>("WHERE target_node_id == " + std::to_string(targetId));
}

std::vector<StorageEdge> SqliteIndexStorage::getEdgesByTargetIds(const std::vector<Id>& targetIds) const
{
	return doGetAll<StorageEdge>("WHERE target_node_id IN (" + utility::join(utility::toStrings(targetIds), ',') + ")");
}

std::vector<StorageEdge> SqliteIndexStorage::getEdgesBySourceOrTargetId(Id id) const
{
	return doGetAll<StorageEdge>("WHERE source_node_id == " + std::to_string(id) + " OR target_node_id == " + std::to_string(id));
}

std::vector<StorageEdge> SqliteIndexStorage::getEdgesByType(int type) const
{
	return doGetAll<StorageEdge>("WHERE type == " + std::to_string(type));
}

std::vector<StorageEdge> SqliteIndexStorage::getEdgesBySourceType(Id sourceId, int type) const
{
	return doGetAll<StorageEdge>("WHERE source_node_id == " + std::to_string(sourceId) + " AND type == " + std::to_string(type));
}

std::vector<StorageEdge> SqliteIndexStorage::getEdgesBySourcesType(const std::vector<Id>& sourceIds, int type) const
{
	return doGetAll<StorageEdge>("WHERE source_node_id IN (" + utility::join(utility::toStrings(sourceIds), ',') + ") AND type == " + std::to_string(type));
}

std::vector<StorageEdge> SqliteIndexStorage::getEdgesByTargetType(Id targetId, int type) const
{
	return doGetAll<StorageEdge>("WHERE target_node_id == " + std::to_string(targetId) + " AND type == " + std::to_string(type));
}

std::vector<StorageEdge> SqliteIndexStorage::getEdgesByTargetsType(const std::vector<Id>& targetIds, int type) const
{
	return doGetAll<StorageEdge>("WHERE target_node_id IN (" + utility::join(utility::toStrings(targetIds), ',') + ") AND type == " + std::to_string(type));
}

StorageNode SqliteIndexStorage::getNodeById(Id id) const
{
	std::vector<StorageNode> candidates = doGetAll<StorageNode>("WHERE id = " + std::to_string(id));

	if (candidates.size() > 0)
	{
		return candidates[0];
	}

	return StorageNode();
}

StorageNode SqliteIndexStorage::getNodeBySerializedName(const std::string& serializedName) const
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

StorageLocalSymbol SqliteIndexStorage::getLocalSymbolByName(const std::string& name) const
{
	return doGetFirst<StorageLocalSymbol>("WHERE name == '" + name + "'");
}

StorageFile SqliteIndexStorage::getFileByPath(const std::string& filePath) const
{
	return doGetFirst<StorageFile>("WHERE file.path == '" + filePath + "'");
}

std::vector<StorageFile> SqliteIndexStorage::getFilesByPaths(const std::vector<FilePath>& filePaths) const
{
	return doGetAll<StorageFile>("WHERE file.path IN ('" + utility::join(utility::toStrings(filePaths), "', '") + "')");
}

std::shared_ptr<TextAccess> SqliteIndexStorage::getFileContentById(Id fileId) const
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

std::shared_ptr<TextAccess> SqliteIndexStorage::getFileContentByPath(const std::string& filePath) const
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

void SqliteIndexStorage::setFileComplete(bool complete, Id fileId)
{
	executeStatement(
		"UPDATE file SET complete = " + std::to_string(complete) + " WHERE id == " + std::to_string(fileId) + ";"
	);
}

void SqliteIndexStorage::setNodeType(int type, Id nodeId)
{
	executeStatement(
		"UPDATE node SET type = " + std::to_string(type) + " WHERE id == " + std::to_string(nodeId) + ";"
	);
}

std::shared_ptr<SourceLocationFile> SqliteIndexStorage::getSourceLocationsForFile(const FilePath& filePath) const
{
	std::shared_ptr<SourceLocationFile> ret = std::make_shared<SourceLocationFile>(filePath, true, false);

	const StorageFile file = getFileByPath(filePath.str());
	if (file.id == 0) // early out
	{
		return ret;
	}

	ret->setIsComplete(file.complete);

	std::vector<Id> sourceLocationIds;
	std::unordered_map<Id, StorageSourceLocation> sourceLocationIdToData;
	for (const StorageSourceLocation& storageLocation:
		doGetAll<StorageSourceLocation>("WHERE file_node_id == " + std::to_string(file.id)))
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

std::vector<StorageOccurrence> SqliteIndexStorage::getOccurrencesForLocationId(Id locationId) const
{
	std::vector<Id> locationIds {locationId};
	return getOccurrencesForLocationIds(locationIds);
}

std::vector<StorageOccurrence> SqliteIndexStorage::getOccurrencesForLocationIds(const std::vector<Id>& locationIds) const
{
	return doGetAll<StorageOccurrence>("WHERE source_location_id IN (" + utility::join(utility::toStrings(locationIds), ',') + ")");
}

std::vector<StorageOccurrence> SqliteIndexStorage::getOccurrencesForElementIds(const std::vector<Id>& elementIds) const
{
	return doGetAll<StorageOccurrence>("WHERE element_id IN (" + utility::join(utility::toStrings(elementIds), ',') + ")");
}

StorageComponentAccess SqliteIndexStorage::getComponentAccessByNodeId(Id nodeId) const
{
	return doGetFirst<StorageComponentAccess>("WHERE node_id == " + std::to_string(nodeId));
}

std::vector<StorageComponentAccess> SqliteIndexStorage::getComponentAccessesByNodeIds(const std::vector<Id>& nodeIds) const
{
	return doGetAll<StorageComponentAccess>("WHERE node_id IN (" + utility::join(utility::toStrings(nodeIds), ',') + ")");
}

std::vector<StorageCommentLocation> SqliteIndexStorage::getCommentLocationsInFile(const FilePath& filePath) const
{
	Id fileNodeId = getFileByPath(filePath.str()).id;
	return doGetAll<StorageCommentLocation>("WHERE file_node_id == " + std::to_string(fileNodeId));
}

int SqliteIndexStorage::getNodeCount() const
{
	return executeStatementScalar("SELECT COUNT(*) FROM node;");
}

int SqliteIndexStorage::getEdgeCount() const
{
	return executeStatementScalar("SELECT COUNT(*) FROM edge;");
}

int SqliteIndexStorage::getFileCount() const
{
	return executeStatementScalar("SELECT COUNT(*) FROM file;");
}

int SqliteIndexStorage::getCompletedFileCount() const
{
	return executeStatementScalar("SELECT COUNT(*) FROM file WHERE complete = 1;");
}

int SqliteIndexStorage::getFileLineSum() const
{
	return executeStatementScalar("SELECT SUM(line_count) FROM file;");
}

int SqliteIndexStorage::getSourceLocationCount() const
{
	return executeStatementScalar("SELECT COUNT(*) FROM source_location;");
}

std::vector<std::pair<int, SqliteDatabaseIndex>> SqliteIndexStorage::getIndices() const
{
	std::vector<std::pair<int, SqliteDatabaseIndex>> indices;
	indices.push_back(std::make_pair(
		STORAGE_MODE_WRITE,
		SqliteDatabaseIndex("edge_multipart_index", "edge(type, source_node_id, target_node_id)")
	));
	indices.push_back(std::make_pair(
		STORAGE_MODE_WRITE | STORAGE_MODE_READ | STORAGE_MODE_CLEAR,
		SqliteDatabaseIndex("node_serialized_name_index", "node(serialized_name)")
	));
	indices.push_back(std::make_pair(
		STORAGE_MODE_WRITE,
		SqliteDatabaseIndex("local_symbol_name_index", "local_symbol(name)")
	));
	indices.push_back(std::make_pair(
		STORAGE_MODE_READ | STORAGE_MODE_CLEAR,
		SqliteDatabaseIndex("source_location_file_node_id_index", "source_location(file_node_id)")
	));
	indices.push_back(std::make_pair(
		STORAGE_MODE_WRITE,
		SqliteDatabaseIndex("source_location_all_data_index", "source_location(file_node_id, start_line, start_column, end_line, end_column, type)")
	));
	indices.push_back(std::make_pair(
		STORAGE_MODE_WRITE,
		SqliteDatabaseIndex("comment_location_all_data_index", "comment_location(file_node_id, start_line, start_column, end_line, end_column)")
	));
	indices.push_back(std::make_pair(
		STORAGE_MODE_WRITE,
		SqliteDatabaseIndex("error_all_data_index", "error(message, fatal, file_path, line_number, column_number)")
	));
	indices.push_back(std::make_pair(
		STORAGE_MODE_WRITE,
		SqliteDatabaseIndex("file_path_index", "file(path)")
	));
	indices.push_back(std::make_pair(
		STORAGE_MODE_READ | STORAGE_MODE_CLEAR,
		SqliteDatabaseIndex("occurrence_element_id_index", "occurrence(element_id)")
	));
	indices.push_back(std::make_pair(
		STORAGE_MODE_CLEAR,
		SqliteDatabaseIndex("occurrence_source_location_id_index", "occurrence(source_location_id)")
	));
	indices.push_back(std::make_pair(
		STORAGE_MODE_WRITE | STORAGE_MODE_READ | STORAGE_MODE_CLEAR,
		SqliteDatabaseIndex("component_access_node_id_index", "component_access(node_id)")
	));
	return indices;
}

void SqliteIndexStorage::clearTables()
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

void SqliteIndexStorage::setupTables()
{
	try
	{
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
				"complete INTEGER, "
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
			"CREATE TABLE IF NOT EXISTS occurrence("
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

		throw(std::exception());

		// todo: cancel project creation and destroy created files, display message
	}
}

size_t SqliteIndexStorage::getStaticStorageVersion() const
{
	return s_storageVersion;
}

template <>
std::vector<StorageEdge> SqliteIndexStorage::doGetAll<StorageEdge>(const std::string& query) const
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
std::vector<StorageNode> SqliteIndexStorage::doGetAll<StorageNode>(const std::string& query) const
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
std::vector<StorageSymbol> SqliteIndexStorage::doGetAll<StorageSymbol>(const std::string& query) const
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
std::vector<StorageFile> SqliteIndexStorage::doGetAll<StorageFile>(const std::string& query) const
{
	CppSQLite3Query q = executeQuery(
		"SELECT id, path, modification_time, complete FROM file " + query + ";"
	);

	std::vector<StorageFile> files;
	while (!q.eof())
	{
		const Id id							= q.getIntField(0, 0);
		const std::string filePath			= q.getStringField(1, "");
		const std::string modificationTime	= q.getStringField(2, "");
		const bool complete					= q.getIntField(3, 0);

		if (id != 0)
		{
			files.push_back(StorageFile(id, filePath, modificationTime, complete));
		}
		q.nextRow();
	}

	return files;
}

template <>
std::vector<StorageLocalSymbol> SqliteIndexStorage::doGetAll<StorageLocalSymbol>(const std::string& query) const
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
std::vector<StorageSourceLocation> SqliteIndexStorage::doGetAll<StorageSourceLocation>(const std::string& query) const
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
std::vector<StorageOccurrence> SqliteIndexStorage::doGetAll<StorageOccurrence>(const std::string& query) const
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
std::vector<StorageComponentAccess> SqliteIndexStorage::doGetAll<StorageComponentAccess>(const std::string& query) const
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
			componentAccesses.push_back(StorageComponentAccess(id, nodeId, type));
		}

		q.nextRow();
	}
	return componentAccesses;
}

template <>
std::vector<StorageCommentLocation> SqliteIndexStorage::doGetAll<StorageCommentLocation>(const std::string& query) const
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
std::vector<StorageError> SqliteIndexStorage::doGetAll<StorageError>(const std::string& query) const
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
