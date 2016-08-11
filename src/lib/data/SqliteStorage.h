#ifndef SQLITE_STORAGE_H
#define SQLITE_STORAGE_H

#include <memory>
#include <string>
#include <vector>

#include "sqlite/CppSQLite3.h"

#include "utility/file/FilePath.h"
#include "utility/types.h"
#include "data/location/TokenLocationFile.h"
#include "data/location/TokenLocationCollection.h"
#include "data/name/NameHierarchy.h"
#include "data/StorageTypes.h"

class TextAccess;
class Version;
struct ParseLocation;

class SqliteStorage
{
public:
	SqliteStorage(const FilePath& dbFilePath);
	~SqliteStorage();

	void setup();
	void clear();

	void beginTransaction();
	void commitTransaction();
	void rollbackTransaction();

	FilePath getDbFilePath() const;

	bool isEmpty() const;
	bool isIncompatible() const;
	std::string getProjectSettingsText() const;
	void setProjectSettingsText(std::string text);

	void setVersion();

	Id addEdge(int type, Id sourceNodeId, Id targetNodeId);
	Id addNode(int type, const std::string& serializedName, int definitionType);
	Id addFile(const std::string& serializedName, const std::string& filePath, const std::string& modificationTime);
	Id addLocalSymbol(const std::string& name);
	Id addSourceLocation(Id elementId, Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, int type);
	Id addComponentAccess(Id nodeId, int type);
	Id addCommentLocation(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol);
	Id addError(const std::string& message, bool fatal, bool indexed, const std::string& filePath, uint lineNumber, uint columnNumber);

	void removeElement(Id id);
	void removeElements(const std::vector<Id>& ids);
	void removeElementsWithLocationInFiles(const std::vector<Id>& fileIds);

	void removeErrorsInFiles(const std::vector<FilePath>& filePaths);

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
	std::shared_ptr<TextAccess> getFileContentByPath(const std::string& filePath) const;
	std::shared_ptr<TextAccess> getFileContentById(Id fileId) const;

	void setNodeType(int type, Id nodeId);
	void setNodeDefinitionType(int definitionType, Id nodeId);

	StorageSourceLocation getSourceLocationById(const Id id) const;
	std::shared_ptr<TokenLocationFile> getTokenLocationsForFile(const FilePath& filePath) const;
	std::vector<StorageSourceLocation> getTokenLocationsForElementId(const Id elementId) const;
	std::vector<StorageSourceLocation> getTokenLocationsForElementIds(const std::vector<Id> elementIds) const;

	Id getElementIdByLocationId(Id locationId) const;

	StorageComponentAccess getComponentAccessByNodeId(Id memberEdgeId) const;
	std::vector<StorageComponentAccess> getComponentAccessesByNodeIds(const std::vector<Id>& memberEdgeIds) const;

	void optimizeMemory() const;

	std::vector<ParseLocation> getFullTextSearch(const std::string& searchTerm) const;

	std::vector<StorageCommentLocation> getCommentLocationsInFile(const FilePath& filePath) const;

	std::vector<StorageFile> getAllFiles() const;
	std::vector<StorageNode> getAllNodes() const;
	std::vector<StorageEdge> getAllEdges() const;
	std::vector<StorageLocalSymbol> getAllLocalSymbols() const;
	std::vector<StorageSourceLocation> getAllSourceLocations() const;
	std::vector<StorageComponentAccess> getAllComponentAccesses() const;
	std::vector<StorageCommentLocation> getAllCommentLocations() const;
	std::vector<StorageError> getAllErrors() const;

	int getNodeCount() const;
	int getEdgeCount() const;
	int getFileCount() const;
	int getFileLOCCount() const;
	int getSourceLocationCount() const;

private:
	static const size_t STORAGE_VERSION;

	void clearTables();
	void setupTables();

	bool hasTable(const std::string& tableName) const;

	std::string getMetaValue(const std::string& key) const;
	void insertOrUpdateMetaValue(const std::string& key, const std::string& value);

	size_t getStorageVersion() const;
	void setStorageVersion();

	Version getApplicationVersion() const;
	void setApplicationVersion();

	template <typename ResultType>
	std::vector<ResultType> getAll(const std::string& query) const;

	template <typename ResultType>
	ResultType getFirst(const std::string& query) const
	{
		std::vector<ResultType> results = getAll<ResultType>(query + " LIMIT 1");
		if (results.size() > 0)
		{
			return results[0];
		}
		return ResultType();
	}

	mutable CppSQLite3DB m_database;
	FilePath m_dbFilePath;
};

template <>
std::vector<StorageFile> SqliteStorage::getAll<StorageFile>(const std::string& query) const;
template <>
std::vector<StorageEdge> SqliteStorage::getAll<StorageEdge>(const std::string& query) const;
template <>
std::vector<StorageNode> SqliteStorage::getAll<StorageNode>(const std::string& query) const;
template <>
std::vector<StorageLocalSymbol> SqliteStorage::getAll<StorageLocalSymbol>(const std::string& query) const;
template <>
std::vector<StorageSourceLocation> SqliteStorage::getAll<StorageSourceLocation>(const std::string& query) const;
template <>
std::vector<StorageComponentAccess> SqliteStorage::getAll<StorageComponentAccess>(const std::string& query) const;
template <>
std::vector<StorageCommentLocation> SqliteStorage::getAll<StorageCommentLocation>(const std::string& query) const;
template <>
std::vector<StorageError> SqliteStorage::getAll<StorageError>(const std::string& query) const;


#endif // SQLITE_STORAGE_H
