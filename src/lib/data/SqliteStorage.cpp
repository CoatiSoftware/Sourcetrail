#include "data/SqliteStorage.h"

#include "data/graph/Node.h"
#include "data/location/TokenLocation.h"
#include "utility/logging/logging.h"

SqliteStorage::SqliteStorage(const std::string& dbFilePath)
{
	m_database.open(dbFilePath.c_str());
	setup();
}

SqliteStorage::~SqliteStorage()
{
	m_database.close();
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

Id SqliteStorage::addNode(int type, Id nameId)
{
	m_database.execDML(
		"INSERT INTO element(id) VALUES(NULL);"
	);
	Id id = m_database.lastRowId();

	m_database.execDML((
		"INSERT INTO node(id, type, name_id) VALUES("
		+ std::to_string(id) + ", " + std::to_string(type) + ", " + std::to_string(nameId) + ");"
	).c_str());

	return id;
}

Id SqliteStorage::addFile(Id nameId, const std::string& filePath, const std::string& modificationTime)
{
	Id id = addNode(Node::NODE_FILE, nameId);

	m_database.execDML((
		"INSERT INTO file(id, path, modification_time) VALUES("
		+ std::to_string(id) + ", '" + filePath + "', '" + modificationTime + "');"
	).c_str());

	return id;
}

int SqliteStorage::addSourceLocation(Id elementId, Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, bool isScope)
{
	m_database.execDML((
		"INSERT INTO source_location(id, element_id, file_node_id, start_line, start_column, end_line, end_column, is_scope) "
		"VALUES(NULL, " + std::to_string(elementId) + ", " + std::to_string(fileNodeId) + ", "
		+ std::to_string(startLine) + ", " + std::to_string(startCol) + ", "
		+ std::to_string(endLine) + ", " + std::to_string(endCol) + ", " + std::to_string(isScope) + ");"
	).c_str());

	return m_database.lastRowId();
}

Id SqliteStorage::addNameHierarchyElement(const std::string& name)
{
	m_database.execDML((
		"INSERT INTO name_hierarchy_element(id, name, parent_id) "
		"VALUES(NULL, '" + name + "', NULL);"
	).c_str());

	return m_database.lastRowId();
}

Id SqliteStorage::addNameHierarchyElement(const std::string& name, Id parentId)
{
	m_database.execDML((
		"INSERT INTO name_hierarchy_element(id, name, parent_id) "
		"VALUES (NULL, '" + name + "', " + std::to_string(parentId) + ");"
	).c_str());

	return m_database.lastRowId();
}

Id SqliteStorage::addComponentAccess(Id memberEdgeId, int type)
{
	m_database.execDML((
		"INSERT INTO component_access(id, edge_id, type) "
		"VALUES (NULL, '" + std::to_string(memberEdgeId) + "', " + std::to_string(type) + ");"
	).c_str());

	return m_database.lastRowId();
}

void SqliteStorage::removeElement(Id id)
{
	m_database.execDML((
		"DELETE FROM element WHERE id == " + std::to_string(id) + ";"
	).c_str());
}

void SqliteStorage::removeNameHierarchyElement(Id id)
{
	m_database.execDML((
		"DELETE FROM name_hierarchy_element WHERE id == " + std::to_string(id) + ";"
	).c_str());
}

void SqliteStorage::removeElementsWithLocationInFile(Id fileId)
{
	m_database.execDML((
		"DELETE FROM element WHERE id IN (SELECT element_id FROM source_location WHERE source_location.file_node_id == " + std::to_string(fileId) + ");"
	).c_str());
}

void SqliteStorage::removeFile(Id id)
{
	if (isFile(id))
	{
		m_database.execDML((
			"DELETE FROM element WHERE id == " + std::to_string(id) + ";"
		).c_str());
	}
	else
	{
		LOG_WARNING("Removing file from DB failed since there is no file element with id " + std::to_string(id));
	}
}

void SqliteStorage::removeUnusedNameHierarchyElements()
{
	m_database.execDML(
		"DELETE FROM name_hierarchy_element WHERE NOT EXISTS (SELECT * FROM node where node.name_id == name_hierarchy_element.id);"
	);
}

std::vector<StorageNode> SqliteStorage::getAllNodes() const
{
	std::vector<StorageNode> nodes;

	CppSQLite3Query q = m_database.execQuery(
		"SELECT id, type, name_id FROM node;"
	);

	while (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const int type = q.getIntField(1, -1);
		const Id nameId = q.getIntField(2, 0);

		if (id != 0 && type != -1 && nameId != 0)
		{
			nodes.push_back(StorageNode(id, type, nameId));
		}

		q.nextRow();
	}

	return nodes;
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

std::vector<StorageEdge> SqliteStorage::getEdgesBySourceId(Id sourceId) const
{
	std::vector<StorageEdge> edges;

	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, type, target_node_id FROM edge WHERE "
			"source_node_id == " + std::to_string(sourceId) + ";"
	).c_str());

	while (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const int type = q.getIntField(1, -1);
		const Id targetId = q.getIntField(2, 0);

		if (id != 0 && type != -1 && targetId != 0)
		{
			edges.push_back(StorageEdge(id, type, sourceId, targetId));
		}

		q.nextRow();
	}
	return edges;
}

std::vector<StorageEdge> SqliteStorage::getEdgesByTargetId(Id targetId) const
{
	std::vector<StorageEdge> edges;

	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, type, source_node_id FROM edge WHERE "
			"target_node_id == " + std::to_string(targetId) + ";"
	).c_str());

	while (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const int type = q.getIntField(1, -1);
		const Id sourceId = q.getIntField(2, 0);

		if (id != 0 && type != -1 && targetId != 0)
		{
			edges.push_back(StorageEdge(id, type, sourceId, targetId));
		}

		q.nextRow();
	}
	return edges;
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

StorageNode SqliteStorage::getNodeById(Id id) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT type, name_id FROM node WHERE id == " + std::to_string(id) + ";"
	).c_str());

	if (!q.eof())
	{
		const int type = q.getIntField(0, -1);
		const Id nameId = q.getIntField(1, 0);

		if (type != -1 && nameId != 0 )
		{
			return StorageNode(id, type, nameId);
		}
	}
	return StorageNode(0, -1, 0);
}

StorageNode SqliteStorage::getNodeByNameId(Id nameId) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, type FROM node WHERE name_id == " + std::to_string(nameId) + ";"
	).c_str());

	if (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const int type = q.getIntField(1, -1);

		if (id != 0 && type != -1)
		{
			return StorageNode(id, type, nameId);
		}
	}
	return StorageNode(0, -1, 0);
}

StorageNode SqliteStorage::getNodeByName(const std::string& nodeName) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT node.id, node.type, node.name_id FROM node INNER JOIN name_hierarchy_element ON node.name_id = name_hierarchy_element.id WHERE name_hierarchy_element.name = '" + nodeName + "';"
	).c_str());

	if (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const int type = q.getIntField(1, -1);
		const Id nameId = q.getIntField(2, 0);

		if (id != 0 && type != -1 && nameId != 0)
		{
			return StorageNode(id, type, nameId);
		}
	}
	return StorageNode(0, -1, 0);
}

StorageFile SqliteStorage::getFileById(const Id id) const
{
	return getFirstFile(
		"SELECT node.id, node.name_id, file.path, file.modification_time FROM node INNER JOIN file ON node.id = file.id "
		"WHERE node.id == " + std::to_string(id) + ";"
	);
}

StorageFile SqliteStorage::getFileByName(const std::string& fileName) const
{
	Id nameId = getNameHierarchyElementIdByName(fileName);

	StorageFile storageFile(0, 0, "", "");

	if (nameId != 0)
	{
		storageFile = getFirstFile(
			"SELECT node.id, node.name_id, file.path, file.modification_time FROM node INNER JOIN file ON node.id = file.id "
			"WHERE node.name_id == " + std::to_string(nameId) + ";"
		);
	}

	return storageFile;
}

std::vector<StorageFile> SqliteStorage::getAllFiles() const
{
	return getAllFiles("SELECT file.id, node.name_id, file.path, file.modification_time FROM file INNER JOIN node ON file.id = node.id;");
}

void SqliteStorage::setNodeType(int type, Id nodeId)
{
	m_database.execDML((
		"UPDATE node SET type = " + std::to_string(type) + " WHERE id == " + std::to_string(nodeId) + ";"
	).c_str());
}

Id SqliteStorage::getNameHierarchyElementIdByName(const std::string& name) const
{
	return getFirstResult<Id>(
		"SELECT id FROM name_hierarchy_element WHERE name == '" + name + "';"
	);
}

Id SqliteStorage::getNameHierarchyElementIdByName(const std::string& name, Id parentId) const
{
	return getFirstResult<Id>(
		"SELECT id FROM name_hierarchy_element WHERE name == '" + name + "' AND parent_id == " + std::to_string(parentId) + ";"
	);
}

Id SqliteStorage::getNameHierarchyElementIdByNodeId(const Id nodeId) const
{
	return getFirstResult<Id>(
		"SELECT name_id FROM node WHERE id == " + std::to_string(nodeId) + ";"
	);
}

std::vector<StorageNameHierarchyElement> SqliteStorage::getAllNameHierarchyElements() const
{
	std::vector<StorageNameHierarchyElement> ret;

	CppSQLite3Query q = m_database.execQuery(
		"SELECT * FROM name_hierarchy_element;"
	);

	while (!q.eof())
	{
		const Id id				= q.getIntField(0, 0);
		const std::string name	= q.getStringField(1, "");
		const Id parent_id		= q.getIntField(2, 0);


		if (id != 0 && name != "" && parent_id != 0)
		{
			ret.push_back(StorageNameHierarchyElement(id, name, parent_id));
		}
		q.nextRow();
	}

	return ret;
}

NameHierarchy SqliteStorage::getNameHierarchyById(const Id id) const
{
	CppSQLite3Query q = m_database.execQuery((
		"SELECT name, parent_id FROM name_hierarchy_element WHERE id == " + std::to_string(id) + ";"
	).c_str());

	const std::string elementName = q.getStringField(0, "");
	const Id parentId = q.getIntField(1, 0);

	NameHierarchy nameHierarchy = (parentId > 0) ? getNameHierarchyById(parentId) : NameHierarchy();

	if (elementName.size() > 0)
	{
		nameHierarchy.push(std::make_shared<NameElement>(elementName));
	}

	return nameHierarchy;
}

StorageSourceLocation SqliteStorage::getSourceLocationById(const Id id) const
{
	return getFirstSourceLocation(
		"SELECT id, element_id, file_node_id, start_line, start_column, end_line, end_column, is_scope FROM source_location WHERE id == " + std::to_string(id) + ";"
	);
}

std::vector<StorageSourceLocation> SqliteStorage::getAllSourceLocations() const
{
	return getAllSourceLocations(
		"SELECT * FROM source_location;"
	);
}

std::shared_ptr<TokenLocationFile> SqliteStorage::getTokenLocationsForFile(const FilePath& filePath) const
{
	std::shared_ptr<TokenLocationFile> ret = std::make_shared<TokenLocationFile>(filePath);

	const Id fileNodeId = getNodeByName(filePath.fileName()).id;
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
	std::vector<StorageSourceLocation> locations;

	CppSQLite3Query q = m_database.execQuery((
		"SELECT id, file_node_id, start_line, start_column, end_line, end_column, is_scope FROM source_location WHERE element_id == " + std::to_string(elementId) + ";"
	).c_str());

	while (!q.eof())
	{
		const Id id = q.getIntField(0, 0);
		const Id fileNodeId = q.getIntField(1, 0);
		const int startLineNumber = q.getIntField(2, -1);
		const int startColNumber = q.getIntField(3, -1);
		const int endLineNumber = q.getIntField(4, -1);
		const int endColNumber = q.getIntField(5, -1);
		const int isScope = q.getIntField(6, -1);

		if (id != 0 && fileNodeId != 0 && startLineNumber != -1 && startColNumber != -1 && endLineNumber != -1 && endColNumber != -1 && isScope != -1)
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
			q.getIntField(0, 0),
			q.getIntField(1, 0),
			q.getIntField(2, 0)
		);
	}

	return StorageComponentAccess(0, 0, 0);
}

int SqliteStorage::getNodeCount() const
{
	return m_database.execScalar("SELECT COUNT(*) from node;");
}

int SqliteStorage::getEdgeCount() const
{
	return m_database.execScalar("SELECT COUNT(*) from edge;");
}

int SqliteStorage::getNameHierarchyElementCount() const
{
	return m_database.execScalar("SELECT COUNT(*) from name_hierarchy_element;");
}

void SqliteStorage::clearTables()
{
	m_database.execDML("DROP TABLE IF EXISTS main.source_location;");
	m_database.execDML("DROP TABLE IF EXISTS main.name_hierarchy_element;");
	m_database.execDML("DROP TABLE IF EXISTS main.file;");
	m_database.execDML("DROP TABLE IF EXISTS main.node;");
	m_database.execDML("DROP TABLE IF EXISTS main.edge;");
	m_database.execDML("DROP TABLE IF EXISTS main.element;");
}

void SqliteStorage::setupTables()
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
			"name_id INTEGER NOT NULL, "
			"PRIMARY KEY(id), "
			"FOREIGN KEY(id) REFERENCES element(id) ON DELETE CASCADE, "
			"FOREIGN KEY(name_id) REFERENCES name_hierarchy_element(id) ON DELETE CASCADE);" // maybe use restrict here
	);

	m_database.execDML(
		"CREATE TABLE IF NOT EXISTS file("
			"id INTEGER NOT NULL, "
			"path TEXT, "
			"modification_time TEXT, "
			"PRIMARY KEY(id), "
			"FOREIGN KEY(id) REFERENCES node(id) ON DELETE CASCADE);"
	);

	m_database.execDML(
		"CREATE TABLE IF NOT EXISTS name_hierarchy_element("
			"id INTEGER NOT NULL, "
			"name TEXT, "
			"parent_id INTEGER, "
			"PRIMARY KEY(id), "
			"FOREIGN KEY(parent_id) REFERENCES name_hierarchy_element(id) ON DELETE CASCADE);" // maybe use restrict here
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

	m_database.execDML(
		"CREATE TABLE IF NOT EXISTS component_access("
			"id INTEGER NOT NULL, "
			"edge_id INTEGER, "
			"type INTEGER NOT NULL, "
			"PRIMARY KEY(id), "
			"FOREIGN KEY(edge_id) REFERENCES element(id) ON DELETE CASCADE);"
	);
}

StorageFile SqliteStorage::getFirstFile(const std::string& query) const
{
	CppSQLite3Query q = m_database.execQuery(query.c_str());

	if (!q.eof())
	{
		const Id id							= q.getIntField(0, 0);
		const Id nameId						= q.getIntField(1, 0);
		const std::string filePath			= q.getStringField(2, "");
		const std::string modificationTime	= q.getStringField(3, "");

		if (id != 0 && nameId != 0)
		{
			return StorageFile(id, nameId, filePath, modificationTime);
		}
	}
	return StorageFile(0, 0, "", "");
}

std::vector<StorageFile> SqliteStorage::getAllFiles(const std::string& query) const
{
	std::vector<StorageFile> files;

	CppSQLite3Query q = m_database.execQuery(query.c_str());

	while (!q.eof())
	{
		const Id id							= q.getIntField(0, 0);
		const Id nameId						= q.getIntField(1, 0);
		const std::string filePath			= q.getStringField(2, "");
		const std::string modificationTime	= q.getStringField(3, "");

		if (id != 0 && nameId != 0)
		{
			files.push_back(StorageFile(id, nameId, filePath, modificationTime));
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

std::vector<StorageSourceLocation> SqliteStorage::getAllSourceLocations(const std::string& query) const
{
	std::vector<StorageSourceLocation> sourceLocations;

	CppSQLite3Query q = m_database.execQuery(query.c_str());

	while (!q.eof())
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
			sourceLocations.push_back(StorageSourceLocation(
				id, elementId, fileNodeId, startLineNumber, startColNumber, endLineNumber, endColNumber, isScope
			));
		}

		q.nextRow();
	}

	return sourceLocations;
}
