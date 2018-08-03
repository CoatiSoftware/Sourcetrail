#include "data/storage/sqlite/SqliteIndexStorage.h"

#include <unordered_map>

#include "utility/logging/logging.h"
#include "utility/text/TextAccess.h"
#include "utility/utilityString.h"

#include "data/location/SourceLocationCollection.h"
#include "data/location/SourceLocationFile.h"

const size_t SqliteIndexStorage::s_storageVersion = 17;

SqliteIndexStorage::SqliteIndexStorage(const FilePath& dbFilePath)
	: SqliteStorage(dbFilePath.getCanonical())
{
}

size_t SqliteIndexStorage::getStaticVersion() const
{
	return s_storageVersion;
}

std::string SqliteIndexStorage::getProjectSettingsText() const
{
	return getMetaValue("project_settings");
}

void SqliteIndexStorage::setProjectSettingsText(std::string text)
{
	insertOrUpdateMetaValue("project_settings", text);
}

StorageNode SqliteIndexStorage::addNode(const StorageNodeData& data)
{
	Id id = 0;
	{
		executeStatement(m_insertElementStmt);
		id = m_database.lastRowId();
		m_insertElementStmt.reset();
	}
	{
		m_inserNodeStmt.bind(1, int(id));
		m_inserNodeStmt.bind(2, data.type);
		m_inserNodeStmt.bind(3, utility::encodeToUtf8(data.serializedName).c_str());
		executeStatement(m_inserNodeStmt);
		m_inserNodeStmt.reset();
	}
	return StorageNode(id, data);
}

void SqliteIndexStorage::addSymbol(const StorageSymbol& data)
{
	m_insertSymbolStmt.bind(1, int(data.id));
	m_insertSymbolStmt.bind(2, data.definitionKind);
	executeStatement(m_insertSymbolStmt);
	m_insertSymbolStmt.reset();
}

void SqliteIndexStorage::addFile(const StorageFile& data)
{
	if (getFileByPath(data.filePath).id != 0)
	{
		return;
	}

	std::shared_ptr<TextAccess> content;
	int lineCount = 0;
	if (data.indexed)
	{
		content = TextAccess::createFromFile(FilePath(data.filePath));
		lineCount = content->getLineCount();
	}

	bool success = false;
	{
		m_insertFileStmt.bind(1, int(data.id));
		m_insertFileStmt.bind(2, utility::encodeToUtf8(data.filePath).c_str());
		m_insertFileStmt.bind(3, data.modificationTime.c_str());
		m_insertFileStmt.bind(4, data.indexed);
		m_insertFileStmt.bind(5, data.complete);
		m_insertFileStmt.bind(6, lineCount);
		success = executeStatement(m_insertFileStmt);
		m_insertFileStmt.reset();
	}

	if (success && content)
	{
		m_insertFileContentStmt.bind(1, int(data.id));
		m_insertFileContentStmt.bind(2, content->getText().c_str());
		executeStatement(m_insertFileContentStmt);
		m_insertFileContentStmt.reset();
	}
}

StorageEdge SqliteIndexStorage::addEdge(const StorageEdgeData& data)
{
	Id id = 0;
	{
		executeStatement(m_insertElementStmt);
		id = m_database.lastRowId();
		m_insertElementStmt.reset();
	}
	{
		m_insertEdgeStmt.bind(1, int(id));
		m_insertEdgeStmt.bind(2, data.type);
		m_insertEdgeStmt.bind(3, int(data.sourceNodeId));
		m_insertEdgeStmt.bind(4, int(data.targetNodeId));
		executeStatement(m_insertEdgeStmt);
		m_insertEdgeStmt.reset();
	}
	return StorageEdge(id, data);
}

StorageLocalSymbol SqliteIndexStorage::addLocalSymbol(const StorageLocalSymbolData& data)
{
	Id id = 0;
	{
		executeStatement(m_insertElementStmt);
		id = m_database.lastRowId();
		m_insertElementStmt.reset();
	}
	{
		m_inserLocalSymbolStmt.bind(1, int(id));
		m_inserLocalSymbolStmt.bind(2, utility::encodeToUtf8(data.name).c_str());
		executeStatement(m_inserLocalSymbolStmt);
		m_inserLocalSymbolStmt.reset();
	}
	return StorageLocalSymbol(id, data);
}

StorageSourceLocation SqliteIndexStorage::addSourceLocation(const StorageSourceLocationData& data)
{
	Id id = 0;
	{
		m_checkSourceLocationExistsStmt.bind(1, int(data.fileNodeId));
		m_checkSourceLocationExistsStmt.bind(2, int(data.startLine));
		m_checkSourceLocationExistsStmt.bind(3, int(data.startCol));
		m_checkSourceLocationExistsStmt.bind(4, int(data.endLine));
		m_checkSourceLocationExistsStmt.bind(5, int(data.endCol));
		m_checkSourceLocationExistsStmt.bind(6, data.type);

		CppSQLite3Query checkQuery = executeQuery(m_checkSourceLocationExistsStmt);
		if (!checkQuery.eof() && checkQuery.numFields() > 0)
		{
			id = checkQuery.getIntField(0, 0);
		}

		m_checkSourceLocationExistsStmt.reset();
	}

	if (id == 0)
	{
		m_insertSourceLocationStmt.bind(1, int(data.fileNodeId));
		m_insertSourceLocationStmt.bind(2, int(data.startLine));
		m_insertSourceLocationStmt.bind(3, int(data.startCol));
		m_insertSourceLocationStmt.bind(4, int(data.endLine));
		m_insertSourceLocationStmt.bind(5, int(data.endCol));
		m_insertSourceLocationStmt.bind(6, data.type);

		const bool success = executeStatement(m_insertSourceLocationStmt);
		if (success)
		{
			id = m_database.lastRowId();
		}

		m_insertSourceLocationStmt.reset();
	}

	return StorageSourceLocation(id, data);
}

bool SqliteIndexStorage::addOccurrence(const StorageOccurrence& data)
{
	{
		m_insertOccurrenceStmt.bind(1, int(data.elementId));
		m_insertOccurrenceStmt.bind(2, int(data.sourceLocationId));
		executeStatement(m_insertOccurrenceStmt);
		m_insertOccurrenceStmt.reset();
	}
	return true;
}

bool SqliteIndexStorage::addOccurrences(const std::vector<StorageOccurrence>& occurrences)
{
	if (!occurrences.empty())
	{
		std::string stmt = "INSERT OR IGNORE INTO occurrence(element_id, source_location_id) VALUES";
		{
			bool isFirst = true;
			for (const StorageOccurrence& occurrence : occurrences)
			{
				if (!isFirst)
				{
					stmt += ",";
				}
				isFirst = false;
				stmt += "(" + std::to_string(occurrence.elementId) + "," + std::to_string(occurrence.sourceLocationId) + ")";
			}
			stmt += ";";
		}
		return executeStatement(stmt);
	}
	return true;
}

StorageComponentAccess SqliteIndexStorage::addComponentAccess(const StorageComponentAccessData& data)
{
	Id id = getComponentAccessByNodeId(data.nodeId).id;

	if (id == 0)
	{
		m_insertComponentAccessStmt.bind(1, int(data.nodeId));
		m_insertComponentAccessStmt.bind(2, data.type);

		const bool success = executeStatement(m_insertComponentAccessStmt);
		if (success)
		{
			id = m_database.lastRowId();
		}

		m_insertComponentAccessStmt.reset();
	}

	return StorageComponentAccess(id, data);
}

StorageCommentLocation SqliteIndexStorage::addCommentLocation(const StorageCommentLocationData& data)
{
	Id id = 0;
	{
		m_checkCommentLocationExistsStmt.bind(1, int(data.fileNodeId));
		m_checkCommentLocationExistsStmt.bind(2, int(data.startLine));
		m_checkCommentLocationExistsStmt.bind(3, int(data.startCol));
		m_checkCommentLocationExistsStmt.bind(4, int(data.endLine));
		m_checkCommentLocationExistsStmt.bind(5, int(data.endCol));

		CppSQLite3Query checkQuery = executeQuery(m_checkCommentLocationExistsStmt);
		if (!checkQuery.eof() && checkQuery.numFields() > 0)
		{
			id = checkQuery.getIntField(0, 0);
		}

		m_checkCommentLocationExistsStmt.reset();
	}

	if (id == 0)
	{
		m_insertCommentLocationStmt.bind(1, int(data.fileNodeId));
		m_insertCommentLocationStmt.bind(2, int(data.startLine));
		m_insertCommentLocationStmt.bind(3, int(data.startCol));
		m_insertCommentLocationStmt.bind(4, int(data.endLine));
		m_insertCommentLocationStmt.bind(5, int(data.endCol));

		const bool success = executeStatement(m_insertCommentLocationStmt);
		if (success)
		{
			id = m_database.lastRowId();
		}

		m_insertCommentLocationStmt.reset();
	}

	return StorageCommentLocation(id, data);
}

StorageError SqliteIndexStorage::addError(const StorageErrorData& data)
{
	const std::wstring sanitizedMessage = utility::replace(data.message, L"'", L"''");

	Id id = 0;
	{
		m_checkErrorExistsStmt.bind(1, utility::encodeToUtf8(sanitizedMessage).c_str());
		m_checkErrorExistsStmt.bind(2, int(data.fatal));
		m_checkErrorExistsStmt.bind(3, utility::encodeToUtf8(data.filePath).c_str());
		m_checkErrorExistsStmt.bind(4, int(data.lineNumber));
		m_checkErrorExistsStmt.bind(5, int(data.columnNumber));

		CppSQLite3Query checkQuery = executeQuery(m_checkErrorExistsStmt);
		if (!checkQuery.eof() && checkQuery.numFields() > 0)
		{
			id = checkQuery.getIntField(0, -1);
		}

		m_checkErrorExistsStmt.reset();
	}

	if (id == 0)
	{
		m_insertErrorStmt.bind(1, utility::encodeToUtf8(sanitizedMessage).c_str());
		m_insertErrorStmt.bind(2, data.fatal);
		m_insertErrorStmt.bind(3, data.indexed);
		m_insertErrorStmt.bind(4, utility::encodeToUtf8(data.filePath).c_str());
		m_insertErrorStmt.bind(5, int(data.lineNumber));
		m_insertErrorStmt.bind(6, int(data.columnNumber));
		m_insertErrorStmt.bind(7, utility::encodeToUtf8(data.translationUnit).c_str());

		const bool success = executeStatement(m_insertErrorStmt);
		if (success)
		{
			id = m_database.lastRowId();
		}

		m_insertErrorStmt.reset();
	}

	return StorageError(id, data);
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
		updateStatusCallback(30);
	}

	// delete all edges originating from element_id_to_clear
	executeStatement(
		"DELETE FROM element WHERE element.id IN (SELECT id FROM edge WHERE source_node_id IN (SELECT id FROM element_id_to_clear))"
	);

	if (updateStatusCallback != nullptr)
	{
		updateStatusCallback(31);
	}

	// remove all non existing ids from element_id_to_clear (they have been cleared by now and we can disregard them)
	executeStatement(
		"DELETE FROM element_id_to_clear WHERE id NOT IN ("
		"	SELECT id FROM element"
		")"
	);

	if (updateStatusCallback != nullptr)
	{
		updateStatusCallback(33);
	}

	// remove all files from element_id_to_clear (they will be cleared later)
	executeStatement(
		"DELETE FROM element_id_to_clear WHERE id IN ("
		"	SELECT id FROM file"
		")"
	);

	if (updateStatusCallback != nullptr)
	{
		updateStatusCallback(34);
	}

	// delete source locations from fileIds (this also deletes the respective occurrences)
	executeStatement(
		"DELETE FROM source_location WHERE file_node_id IN (" + utility::join(utility::toStrings(fileIds), ',') + ");"
	);

	if (updateStatusCallback != nullptr)
	{
		updateStatusCallback(45);
	}

	// remove all ids from element_id_to_clear that still have occurrences
	executeStatement(
		"DELETE FROM element_id_to_clear WHERE id IN ("
		"	SELECT element_id_to_clear.id FROM element_id_to_clear INNER JOIN occurrence ON element_id_to_clear.id = occurrence.element_id"
		")"
	);

	if (updateStatusCallback != nullptr)
	{
		updateStatusCallback(59);
	}

	// remove all ids from element_id_to_clear that still have an edge pointing to them
	executeStatement(
		"DELETE FROM element_id_to_clear WHERE id IN ("
		"	SELECT target_node_id FROM edge"
		")"
	);

	if (updateStatusCallback != nullptr)
	{
		updateStatusCallback(74);
	}

	// delete all elements that are still listed in element_id_to_clear
	executeStatement(
		"DELETE FROM element WHERE EXISTS ("
		"	SELECT * FROM element_id_to_clear WHERE element.id = element_id_to_clear.id"
		")"
	);

	if (updateStatusCallback != nullptr)
	{
		updateStatusCallback(87);
	}

	// cleaning up
	executeStatement("DROP TABLE IF EXISTS main.element_id_to_clear;");

	if (updateStatusCallback != nullptr)
	{
		updateStatusCallback(89);
	}
}

void SqliteIndexStorage::removeAllErrors()
{
	executeStatement(
		"DELETE FROM error;"
	);
}

void SqliteIndexStorage::removeErrorsInFiles(const std::vector<FilePath>& filePaths)
{
	executeStatement(
		"DELETE FROM error WHERE file_path IN ('" + utility::join(utility::toStrings(filePaths), "', '") + "');"
	);
}

bool SqliteIndexStorage::isEdge(Id elementId) const
{
	int count = executeStatementScalar("SELECT count(*) FROM edge WHERE id = " + std::to_string(elementId) + ";", 0);
	return (count > 0);
}

bool SqliteIndexStorage::isNode(Id elementId) const
{
	int count = executeStatementScalar("SELECT count(*) FROM node WHERE id = " + std::to_string(elementId) + ";", 0);
	return (count > 0);
}

bool SqliteIndexStorage::isFile(Id elementId) const
{
	int count = executeStatementScalar("SELECT count(*) FROM file WHERE id = " + std::to_string(elementId) + ";", 0);
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

StorageNode SqliteIndexStorage::getNodeBySerializedName(const std::wstring& serializedName) const
{
	CppSQLite3Statement stmt = m_database.compileStatement(
		"SELECT id, type, serialized_name FROM node WHERE serialized_name == ? LIMIT 1;"
	);

	stmt.bind(1, utility::encodeToUtf8(serializedName).c_str());
	CppSQLite3Query q = executeQuery(stmt);

	if (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const int type = q.getIntField(1, -1);
		const std::string name = q.getStringField(2, "");

		if (id != 0 && type != -1)
		{
			return StorageNode(id, type, utility::decodeFromUtf8(name));
		}
	}

	return StorageNode();
}

StorageLocalSymbol SqliteIndexStorage::getLocalSymbolByName(const std::wstring& name) const
{
	return doGetFirst<StorageLocalSymbol>("WHERE name == '" + utility::encodeToUtf8(name) + "'");
}

StorageFile SqliteIndexStorage::getFileByPath(const std::wstring& filePath) const
{
	return doGetFirst<StorageFile>("WHERE file.path == '" + utility::encodeToUtf8(filePath) + "'");
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

std::shared_ptr<TextAccess> SqliteIndexStorage::getFileContentByPath(const std::wstring& filePath) const
{
	try
	{
		CppSQLite3Query q = executeQuery(
			"SELECT filecontent.content "
				"FROM filecontent "
				"INNER JOIN file ON filecontent.id = file.id "
				"WHERE file.path = '" + utility::encodeToUtf8(filePath) + "';"
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

	return TextAccess::createFromFile(FilePath(filePath));
}

void SqliteIndexStorage::setFileIndexed(Id fileId, bool indexed)
{
	executeStatement(
		"UPDATE file SET indexed = " + std::to_string(indexed) + " WHERE id == " + std::to_string(fileId) + ";"
	);
}

void SqliteIndexStorage::setFileCompleteIfNoError(Id fileId, const std::wstring& filePath, bool complete)
{
	bool fileHasErrors = doGetFirst<StorageError>("WHERE file_path == '" + utility::encodeToUtf8(filePath) + "'").id;
	if (fileHasErrors != complete)
	{
		executeStatement(
			"UPDATE file SET complete = " + std::to_string(complete) + " WHERE id == " + std::to_string(fileId) + ";"
		);
	}
}

void SqliteIndexStorage::setNodeType(int type, Id nodeId)
{
	executeStatement(
		"UPDATE node SET type = " + std::to_string(type) + " WHERE id == " + std::to_string(nodeId) + ";"
	);
}

std::shared_ptr<SourceLocationFile> SqliteIndexStorage::getSourceLocationsForFile(
	const FilePath& filePath, const std::string& query) const
{
	std::shared_ptr<SourceLocationFile> ret = std::make_shared<SourceLocationFile>(filePath, true, false, false);

	const StorageFile file = getFileByPath(filePath.wstr());
	if (file.id == 0) // early out
	{
		return ret;
	}

	ret->setIsComplete(file.complete);
	ret->setIsIndexed(file.indexed);

	std::vector<Id> sourceLocationIds;
	std::unordered_map<Id, StorageSourceLocation> sourceLocationIdToData;
	for (const StorageSourceLocation& storageLocation:
		doGetAll<StorageSourceLocation>("WHERE file_node_id == " + std::to_string(file.id) + " " + query))
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

std::shared_ptr<SourceLocationFile> SqliteIndexStorage::getSourceLocationsForLinesInFile(
	const FilePath& filePath, size_t startLine, size_t endLine) const
{
	return getSourceLocationsForFile(filePath,
		"AND start_line <= " + std::to_string(endLine) + " AND end_line >= " + std::to_string(startLine));
}

std::shared_ptr<SourceLocationFile> SqliteIndexStorage::getSourceLocationsOfTypeInFile(
	const FilePath& filePath, LocationType type) const
{
	return getSourceLocationsForFile(filePath, "AND type == " + std::to_string(locationTypeToInt(type)));
}

std::shared_ptr<SourceLocationCollection> SqliteIndexStorage::getSourceLocationsForElementIds(
	const std::vector<Id>& elementIds) const
{
	std::vector<Id> sourceLocationIds;
	std::map<Id, std::vector<Id>> sourceLocationIdToElementIds;
	for (const StorageOccurrence& occurrence : getOccurrencesForElementIds(elementIds))
	{
		sourceLocationIds.push_back(occurrence.sourceLocationId);
		sourceLocationIdToElementIds[occurrence.sourceLocationId].push_back(occurrence.elementId);
	}

	CppSQLite3Query q = executeQuery(
		"SELECT source_location.id, file.path, source_location.start_line, source_location.start_column, "
			"source_location.end_line, source_location.end_column, source_location.type "
		"FROM source_location INNER JOIN file ON (file.id = source_location.file_node_id) "
		"WHERE source_location.id IN (" + utility::join(utility::toStrings(sourceLocationIds), ',') + ");"
	);

	std::shared_ptr<SourceLocationCollection> ret = std::make_shared<SourceLocationCollection>();

	while (!q.eof())
	{
		const Id id					= q.getIntField(0, 0);
		const std::string filePath 	= q.getStringField(1, "");
		const int startLineNumber	= q.getIntField(2, -1);
		const int startColNumber	= q.getIntField(3, -1);
		const int endLineNumber		= q.getIntField(4, -1);
		const int endColNumber		= q.getIntField(5, -1);
		const int type				= q.getIntField(6, -1);

		if (id != 0 && filePath.size() && startLineNumber != -1 && startColNumber != -1 && endLineNumber != -1 &&
			endColNumber != -1 && type != -1)
		{
			ret->addSourceLocation(
				intToLocationType(type),
				id,
				sourceLocationIdToElementIds[id],
				FilePath(utility::decodeFromUtf8(filePath)),
				startLineNumber,
				startColNumber,
				endLineNumber,
				endColNumber
			);
		}

		q.nextRow();
	}

	return ret;
}

std::vector<StorageOccurrence> SqliteIndexStorage::getOccurrencesForLocationId(Id locationId) const
{
	std::vector<Id> locationIds { locationId };
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
	Id fileNodeId = getFileByPath(filePath.wstr()).id;
	return doGetAll<StorageCommentLocation>("WHERE file_node_id == " + std::to_string(fileNodeId));
}

int SqliteIndexStorage::getNodeCount() const
{
	return executeStatementScalar("SELECT COUNT(*) FROM node;", 0);
}

int SqliteIndexStorage::getEdgeCount() const
{
	return executeStatementScalar("SELECT COUNT(*) FROM edge;", 0);
}

int SqliteIndexStorage::getFileCount() const
{
	return executeStatementScalar("SELECT COUNT(*) FROM file WHERE indexed = 1;", 0);
}

int SqliteIndexStorage::getCompletedFileCount() const
{
	return executeStatementScalar("SELECT COUNT(*) FROM file WHERE indexed = 1 AND complete = 1;", 0);
}

int SqliteIndexStorage::getFileLineSum() const
{
	return executeStatementScalar("SELECT SUM(line_count) FROM file;", 0);
}

int SqliteIndexStorage::getSourceLocationCount() const
{
	return executeStatementScalar("SELECT COUNT(*) FROM source_location;", 0);
}

int SqliteIndexStorage::getErrorCount() const
{
	return executeStatementScalar("SELECT COUNT(*) FROM error;", 0);
}

std::vector<std::pair<int, SqliteDatabaseIndex>> SqliteIndexStorage::getIndices() const
{
	std::vector<std::pair<int, SqliteDatabaseIndex>> indices;
	indices.push_back(std::make_pair(
		STORAGE_MODE_WRITE,
		SqliteDatabaseIndex("edge_multipart_index", "edge(type, source_node_id, target_node_id)")
	));
	indices.push_back(std::make_pair(
		STORAGE_MODE_CLEAR,
		SqliteDatabaseIndex("edge_source_node_id_index", "edge(source_node_id)")
	));
	indices.push_back(std::make_pair(
		STORAGE_MODE_CLEAR,
		SqliteDatabaseIndex("edge_target_node_id_index", "edge(target_node_id)")
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
		STORAGE_MODE_READ,
		SqliteDatabaseIndex("source_location_file_node_id_type_index", "source_location(file_node_id, type)")
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
		STORAGE_MODE_CLEAR,
		SqliteDatabaseIndex("comment_location_file_node_id_index", "comment_location(file_node_id)")
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
		STORAGE_MODE_READ | STORAGE_MODE_CLEAR,
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
				"indexed INTEGER, "
				"complete INTEGER, "
				"line_count INTEGER, "
				"PRIMARY KEY(id), "
				"FOREIGN KEY(id) REFERENCES node(id) ON DELETE CASCADE);"
		);

		m_database.execDML(
			"CREATE TABLE IF NOT EXISTS filecontent("
				"id INTERGER, "
				"content TEXT, "
				"FOREIGN KEY(id) REFERENCES file(id)"
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
				"translation_unit TEXT, "
				"PRIMARY KEY(id));"
		);
	}
	catch (CppSQLite3Exception& e)
	{
		LOG_ERROR(std::to_string(e.errorCode()) + ": " + e.errorMessage());

		throw(std::exception());
	}
}

void SqliteIndexStorage::setupPrecompiledStatements()
{
	try
	{
		m_insertElementStmt = m_database.compileStatement(
			"INSERT INTO element(id) VALUES(NULL);"
		);
		m_insertEdgeStmt = m_database.compileStatement(
			"INSERT INTO edge(id, type, source_node_id, target_node_id) VALUES(?, ?, ?, ?);"
		);
		m_inserNodeStmt = m_database.compileStatement(
			"INSERT INTO node(id, type, serialized_name) VALUES(?, ?, ?);"
		);
		m_insertSymbolStmt = m_database.compileStatement(
			"INSERT INTO symbol(id, definition_kind) VALUES(?, ?);"
		);
		m_insertFileStmt = m_database.compileStatement(
			"INSERT INTO file(id, path, modification_time, indexed, complete, line_count) VALUES(?, ?, ?, ?, ?, ?);"
		);
		m_insertFileContentStmt = m_database.compileStatement(
			"INSERT INTO filecontent(id, content) VALUES(?, ?);"
		);
		m_inserLocalSymbolStmt = m_database.compileStatement(
			"INSERT INTO local_symbol(id, name) VALUES(?, ?);"
		);
		m_checkSourceLocationExistsStmt = m_database.compileStatement(
			"SELECT id FROM source_location WHERE "
			"file_node_id = ? AND "
			"start_line = ? AND "
			"start_column = ? AND "
			"end_line = ? AND "
			"end_column = ? AND "
			"type = ? "
			"LIMIT 1;"
		);
		m_insertSourceLocationStmt = m_database.compileStatement(
			"INSERT INTO source_location(id, file_node_id, start_line, start_column, end_line, end_column, type) VALUES(NULL, ?, ?, ?, ?, ?, ?);"
		);
		m_insertOccurrenceStmt = m_database.compileStatement(
			"INSERT OR IGNORE INTO occurrence(element_id, source_location_id) VALUES(?, ?);"
		);
		m_insertComponentAccessStmt = m_database.compileStatement(
			"INSERT INTO component_access(id, node_id, type) VALUES(NULL, ?, ?);"
		);
		m_checkCommentLocationExistsStmt = m_database.compileStatement(
			"SELECT id FROM comment_location WHERE "
			"file_node_id = ? AND "
			"start_line == ? AND "
			"start_column == ? AND "
			"end_line == ? AND "
			"end_column == ? "
			"LIMIT 1;"
		);
		m_insertCommentLocationStmt = m_database.compileStatement(
			"INSERT INTO comment_location(id, file_node_id, start_line, start_column, end_line, end_column) VALUES(NULL, ?, ?, ?, ?, ?);"
		);
		m_checkErrorExistsStmt = m_database.compileStatement(
			"SELECT id FROM error WHERE "
			"message = ? AND "
			"fatal == ? AND "
			"file_path == ? AND "
			"line_number == ? AND "
			"column_number == ? "
			"LIMIT 1;"
		);
		m_insertErrorStmt = m_database.compileStatement(
			"INSERT INTO error(message, fatal, indexed, file_path, line_number, column_number, translation_unit) VALUES(?, ?, ?, ?, ?, ?, ?);"
		);
	}
	catch (CppSQLite3Exception& e)
	{
		LOG_ERROR(std::to_string(e.errorCode()) + ": " + e.errorMessage());

		throw(std::exception());

		// todo: cancel project creation and destroy created files, display message
	}
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
			edges.emplace_back(id, type, sourceId, targetId);
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
			nodes.emplace_back(id, type, utility::decodeFromUtf8(serializedName));
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
			symbols.emplace_back(id, definitionKind);
		}

		q.nextRow();
	}
	return symbols;
}

template <>
std::vector<StorageFile> SqliteIndexStorage::doGetAll<StorageFile>(const std::string& query) const
{
	CppSQLite3Query q = executeQuery(
		"SELECT id, path, modification_time, indexed, complete FROM file " + query + ";"
	);

	std::vector<StorageFile> files;
	while (!q.eof())
	{
		const Id id							= q.getIntField(0, 0);
		const std::string filePath			= q.getStringField(1, "");
		const std::string modificationTime	= q.getStringField(2, "");
		const bool indexed					= q.getIntField(3, 0);
		const bool complete					= q.getIntField(4, 0);

		if (id != 0)
		{
			files.emplace_back(id, utility::decodeFromUtf8(filePath), modificationTime, indexed, complete);
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
			localSymbols.emplace_back(id, utility::decodeFromUtf8(name));
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
			sourceLocations.emplace_back(id, fileNodeId, startLineNumber, startColNumber, endLineNumber, endColNumber, type);
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
			occurrences.emplace_back(elementId, sourceLocationId);
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
			componentAccesses.emplace_back(id, nodeId, type);
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
			commentLocations.emplace_back(
				id, fileNodeId, startLineNumber, startColNumber, endLineNumber, endColNumber
			);
		}

		q.nextRow();
	}
	return commentLocations;
}

template <>
std::vector<StorageError> SqliteIndexStorage::doGetAll<StorageError>(const std::string& query) const
{
	CppSQLite3Query q = executeQuery(
		"SELECT message, fatal, indexed, file_path, line_number, column_number, translation_unit FROM error " + query + ";"
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
		const std::string translationUnit = q.getStringField(6, "");

		if (lineNumber != -1 && columnNumber != -1)
		{
			errors.emplace_back(
				id, utility::decodeFromUtf8(message), utility::decodeFromUtf8(filePath), lineNumber, columnNumber,
				utility::decodeFromUtf8(translationUnit), fatal, indexed
			);
			id++;
		}

		q.nextRow();
	}

	return errors;
}
