#ifndef SQLITE_STORAGE_H
#define SQLITE_STORAGE_H

#include <memory>
#include <string>
#include <vector>

#include "sqlite/CppSQLite3.h"

#include "utility/file/FilePath.h"
#include "utility/types.h"
#include "data/name/NameHierarchy.h"
#include "data/location/TokenLocationFile.h"
#include "data/location/TokenLocationCollection.h"
#include "data/StorageTypes.h"

class TextAccess;

class SqliteStorage
{
public:
	SqliteStorage(const std::string& dbFilePath);
	~SqliteStorage();

	void setup();
	void clear();

	void beginTransaction();
	void commitTransaction();
	void rollbackTransaction();

	Id addEdge(int type, Id sourceNodeId, Id targetNodeId);
	Id addNode(int type, Id nameId, bool defined);
	Id addFile(Id nameId, const std::string& filePath, const std::string& modificationTime);
	int addSourceLocation(Id elementId, Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, bool isScope);
	Id addNameHierarchyElement(const std::string& name, Id parentId = 0);

	Id addComponentAccess(Id memberEdgeId, int type);
	Id addSignature(Id nodeId, const std::string& signature);

	void removeElement(Id id);
	void removeNameHierarchyElement(Id id);
	void removeFile(Id id);
	void removeFiles(const std::vector<Id>& fileIds);
	void removeUnusedNameHierarchyElements();

	StorageNode getFirstNode() const;
	std::vector<StorageNode> getAllNodes() const;

	bool isEdge(Id elementId) const;
	bool isNode(Id elementId) const;
	bool isFile(Id elementId) const;

	StorageEdge getEdgeById(Id edgeId) const;
	StorageEdge getEdgeBySourceTargetType(Id sourceId, Id targetId, int type) const;
	std::vector<StorageEdge> getEdgesByIds(const std::vector<Id>& edgeIds) const;

	std::vector<StorageEdge> getEdgesBySourceId(Id sourceId) const;
	std::vector<StorageEdge> getEdgesBySourceIds(const std::vector<Id>& sourceIds) const;
	std::vector<StorageEdge> getEdgesByTargetId(Id targetId) const;
	std::vector<StorageEdge> getEdgesByTargetIds(const std::vector<Id>& targetIds) const;
	std::vector<StorageEdge> getEdgesBySourceOrTargetId(Id id) const;

	std::vector<StorageEdge> getEdgesByType(int type) const;
	std::vector<StorageEdge> getEdgesBySourceType(Id sourceId, int type) const;
	std::vector<StorageEdge> getEdgesByTargetType(Id targetId, int type) const;

	StorageNode getNodeById(Id id) const;
	StorageNode getNodeByNameId(Id nameId) const;
	std::vector<StorageNode> getNodesByIds(const std::vector<Id>& nodeIds) const;

	StorageFile getFileById(const Id id) const;
	StorageFile getFileByName(const std::string& fileName) const;
	StorageFile getFileByPath(const std::string& filePath) const;
	std::vector<StorageFile> getAllFiles() const;
	std::shared_ptr<TextAccess> getFileContentByPath(const std::string& filePath) const;

	void setNodeType(int type, Id nodeId);
	void setNodeDefined(bool defined, Id nodeId);

	Id getNameHierarchyElementIdByName(const std::string& name, Id parentId = 0) const;
	Id getNameHierarchyElementIdByNodeId(const Id nodeId) const;

	std::vector<StorageNameHierarchyElement> getAllNameHierarchyElements() const;

	NameHierarchy getNameHierarchyById(const Id id) const;

	StorageSourceLocation getSourceLocationById(const Id id) const;
	std::vector<StorageSourceLocation> getAllSourceLocations() const;
	std::shared_ptr<TokenLocationFile> getTokenLocationsForFile(const FilePath& filePath) const;
	std::vector<StorageSourceLocation> getTokenLocationsForElementId(const Id elementId) const;

	Id getElementIdByLocationId(Id locationId) const;

	StorageComponentAccess getComponentAccessByMemberEdgeId(Id memberEdgeId) const;
	std::vector<StorageComponentAccess> getComponentAccessByMemberEdgeIds(const std::vector<Id>& memberEdgeIds) const;
	Id getNodeIdBySignature(const std::string& signature) const;

	int getNodeCount() const;
	int getEdgeCount() const;
	int getFileCount() const;
	int getNameHierarchyElementCount() const;
	int getSourceLocationCount() const;

private:
	void clearTables();
	void setupTables();

	StorageFile getFirstFile(const std::string& query) const;
	std::vector<StorageFile> getAllFiles(const std::string& query) const;
	StorageSourceLocation getFirstSourceLocation(const std::string& query) const;
	std::vector<StorageSourceLocation> getAllSourceLocations(const std::string& query) const;

	std::vector<StorageEdge> getAllEdges(const std::string& query) const;
	std::vector<StorageNode> getAllNodes(const std::string& query) const;

	template <typename ResultType>
	ResultType getFirstResult(const std::string& query) const;

	mutable CppSQLite3DB m_database;
};

template <typename ResultType>
ResultType SqliteStorage::getFirstResult(const std::string& query) const
{
	CppSQLite3Query q = m_database.execQuery(query.c_str());
	return q.getIntField(0, 0);
}

#endif // SQLITE_STORAGE_H
