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
class Version;

class SqliteStorage
{
public:
	SqliteStorage(const std::string& dbFilePath);
	~SqliteStorage();

	bool init();
	void setup();
	void clear();

	void beginTransaction();
	void commitTransaction();
	void rollbackTransaction();

	Version getVersion() const;
	void setVersion(const Version& version);

	Id addEdge(int type, Id sourceNodeId, Id targetNodeId);
	Id addNode(int type, const std::string& serializedName, int definitionType);
	Id addFile(const std::string& serializedName, const std::string& filePath, const std::string& modificationTime);
	Id addLocalSymbol(const std::string& name);
	Id addSourceLocation(Id elementId, Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, int type);

	Id addComponentAccess(Id memberEdgeId, int type);

	Id addCommentLocation(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol);
	Id addError(const std::string& message, bool fatal, const std::string& filePath, uint lineNumber, uint columnNumber);

	void removeElement(Id id);
	void removeElements(const std::vector<Id>& ids);
	void removeElementsWithLocationInFiles(const std::vector<Id>& fileIds);

	void removeErrorsInFiles(const std::vector<FilePath>& filePaths);

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
	StorageNode getNodeBySerializedName(const std::string& serializedName) const;
	std::vector<StorageNode> getNodesByIds(const std::vector<Id>& nodeIds) const;

	StorageLocalSymbol getLocalSymbolByName(const std::string& name) const;

	StorageFile getFileById(const Id id) const;
	StorageFile getFileByPath(const FilePath& filePath) const;

	std::vector<StorageFile> getFilesByPaths(const std::vector<FilePath>& filePaths) const;
	std::vector<StorageFile> getAllFiles() const;
	std::shared_ptr<TextAccess> getFileContentByPath(const std::string& filePath) const;

	void setNodeType(int type, Id nodeId);
	void setNodeDefinitionType(int definitionType, Id nodeId);

	StorageSourceLocation getSourceLocationById(const Id id) const;
	std::shared_ptr<TokenLocationFile> getTokenLocationsForFile(const FilePath& filePath) const;
	std::vector<StorageSourceLocation> getTokenLocationsForElementId(const Id elementId) const;
	std::vector<StorageSourceLocation> getTokenLocationsForElementIds(const std::vector<Id> elementIds) const;

	Id getElementIdByLocationId(Id locationId) const;

	StorageComponentAccess getComponentAccessByMemberEdgeId(Id memberEdgeId) const;
	std::vector<StorageComponentAccess> getComponentAccessByMemberEdgeIds(const std::vector<Id>& memberEdgeIds) const;

	std::vector<StorageCommentLocation> getCommentLocationsInFile(const FilePath& filePath) const;
	std::vector<StorageError> getAllErrors() const;
	std::vector<StorageError> getFatalErrors() const;

	int getNodeCount() const;
	int getEdgeCount() const;
	int getFileCount() const;
	int getFileLOCCount() const;
	int getSourceLocationCount() const;

private:
	void clearTables();
	void setupTables();

	bool hasTable(const std::string& tableName) const;

	std::string getMetaValue(const std::string& key) const;
	void insertOrUpdateMetaValue(const std::string& key, const std::string& value);

	StorageFile getFirstFile(const std::string& query) const;
	std::vector<StorageFile> getAllFiles(const std::string& query) const;
	StorageSourceLocation getFirstSourceLocation(const std::string& query) const;

	std::vector<StorageEdge> getAllEdges(const std::string& query) const;
	std::vector<StorageNode> getAllNodes(const std::string& query) const;
	StorageNode getFirstNode(const std::string& query) const;

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
