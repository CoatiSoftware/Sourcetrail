#ifndef SQLITE_INDEX_STORAGE_H
#define SQLITE_INDEX_STORAGE_H

#include <memory>
#include <string>
#include <vector>

#include "data/location/SourceLocationFile.h"
#include "data/name/NameHierarchy.h"
#include "data/StorageTypes.h"
#include "data/SqliteDatabaseIndex.h"
#include "utility/file/FilePath.h"
#include "utility/types.h"
#include "utility/utility.h"
#include "utility/utilityString.h"

#include "data/SqliteStorage.h"

class TextAccess;
class Version;
struct ParseLocation;

class SqliteIndexStorage
	: public SqliteStorage
{
public:
	SqliteIndexStorage(const FilePath& dbFilePath);
	virtual ~SqliteIndexStorage();

	std::string getProjectSettingsText() const;
	void setProjectSettingsText(std::string text);

	Id addEdge(int type, Id sourceNodeId, Id targetNodeId);

	Id addNode(const int type, const std::string& serializedName);
	void addSymbol(const int id, int definitionKind);
	void addFile(const int id, const std::string& filePath, const std::string& modificationTime, bool complete);
	Id addLocalSymbol(const std::string& name);
	Id addSourceLocation(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, int type);
	bool addOccurrence(Id elementId, Id sourceLocationId);
	Id addComponentAccess(Id nodeId, int type);
	Id addCommentLocation(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol);
	Id addError(const std::string& message, const FilePath& filePath, uint lineNumber, uint columnNumber, bool fatal, bool indexed);

	void removeElement(Id id);
	void removeElements(const std::vector<Id>& ids);
	void removeElementsWithLocationInFiles(const std::vector<Id>& fileIds, std::function<void(int)> updateStatusCallback);

	void removeErrorsInFiles(const std::vector<FilePath>& filePaths);

	bool isEdge(Id elementId) const;
	bool isNode(Id elementId) const;
	bool isFile(Id elementId) const;

	StorageEdge getEdgeById(Id edgeId) const;
	StorageEdge getEdgeBySourceTargetType(Id sourceId, Id targetId, int type) const;

	std::vector<StorageEdge> getEdgesBySourceId(Id sourceId) const;
	std::vector<StorageEdge> getEdgesBySourceIds(const std::vector<Id>& sourceIds) const;
	std::vector<StorageEdge> getEdgesByTargetId(Id targetId) const;
	std::vector<StorageEdge> getEdgesByTargetIds(const std::vector<Id>& targetIds) const;
	std::vector<StorageEdge> getEdgesBySourceOrTargetId(Id id) const;

	std::vector<StorageEdge> getEdgesByType(int type) const;
	std::vector<StorageEdge> getEdgesBySourceType(Id sourceId, int type) const;
	std::vector<StorageEdge> getEdgesBySourcesType(const std::vector<Id>& sourceIds, int type) const;
	std::vector<StorageEdge> getEdgesByTargetType(Id targetId, int type) const;
	std::vector<StorageEdge> getEdgesByTargetsType(const std::vector<Id>& targetIds, int type) const;

	bool checkEdgeExists(Id edgeId) const;

	StorageNode getNodeById(Id id) const;
	StorageNode getNodeBySerializedName(const std::string& serializedName) const;

	StorageLocalSymbol getLocalSymbolByName(const std::string& name) const;

	StorageFile getFileByPath(const std::string& filePath) const;

	std::vector<StorageFile> getFilesByPaths(const std::vector<FilePath>& filePaths) const;
	std::shared_ptr<TextAccess> getFileContentByPath(const std::string& filePath) const;
	std::shared_ptr<TextAccess> getFileContentById(Id fileId) const;

	void setFileComplete(bool complete, Id fileId);
	void setNodeType(int type, Id nodeId);

	StorageSourceLocation getSourceLocationByAll(const Id fileNodeId, const uint startLine, const uint startCol, const uint endLine, const uint endCol, const int type) const;
	std::shared_ptr<SourceLocationFile> getSourceLocationsForFile(const FilePath& filePath) const;

	std::vector<StorageOccurrence> getOccurrencesForLocationId(Id locationId) const;
	std::vector<StorageOccurrence> getOccurrencesForLocationIds(const std::vector<Id>& locationIds) const;
	std::vector<StorageOccurrence> getOccurrencesForElementIds(const std::vector<Id>& elementIds) const;

	StorageComponentAccess getComponentAccessByNodeId(Id memberEdgeId) const;
	std::vector<StorageComponentAccess> getComponentAccessesByNodeIds(const std::vector<Id>& memberEdgeIds) const;

	std::vector<StorageCommentLocation> getCommentLocationsInFile(const FilePath& filePath) const;

	template <typename ResultType>
	std::vector<ResultType> getAll() const
	{
		return doGetAll<ResultType>("");
	}

	template <typename ResultType>
	ResultType getFirstById(const Id id) const
	{
		if (id != 0)
		{
			return doGetFirst<ResultType>("WHERE id == " + std::to_string(id));
		}
		return ResultType();
	}

	template <typename ResultType>
	std::vector<ResultType> getAllByIds(const std::vector<Id>& ids) const
	{
		if (ids.size())
		{
			return doGetAll<ResultType>("WHERE id IN (" + utility::join(utility::toStrings(ids), ',') + ")");
		}
		return std::vector<ResultType>();
	}

	int getNodeCount() const;
	int getEdgeCount() const;
	int getFileCount() const;
	int getCompletedFileCount() const;
	int getFileLineSum() const;
	int getSourceLocationCount() const;

private:
	static const size_t s_storageVersion;

	virtual size_t getStaticStorageVersion() const;
	virtual std::vector<std::pair<int, SqliteDatabaseIndex>> getIndices() const;
	virtual void clearTables();
	virtual void setupTables();

	template <typename ResultType>
	std::vector<ResultType> doGetAll(const std::string& query) const;

	template <typename ResultType>
	ResultType doGetFirst(const std::string& query) const
	{
		std::vector<ResultType> results = doGetAll<ResultType>(query + " LIMIT 1");
		if (results.size() > 0)
		{
			return results[0];
		}
		return ResultType();
	}
};

template <>
std::vector<StorageEdge> SqliteIndexStorage::doGetAll<StorageEdge>(const std::string& query) const;
template <>
std::vector<StorageNode> SqliteIndexStorage::doGetAll<StorageNode>(const std::string& query) const;
template <>
std::vector<StorageSymbol> SqliteIndexStorage::doGetAll<StorageSymbol>(const std::string& query) const;
template <>
std::vector<StorageFile> SqliteIndexStorage::doGetAll<StorageFile>(const std::string& query) const;
template <>
std::vector<StorageLocalSymbol> SqliteIndexStorage::doGetAll<StorageLocalSymbol>(const std::string& query) const;
template <>
std::vector<StorageSourceLocation> SqliteIndexStorage::doGetAll<StorageSourceLocation>(const std::string& query) const;
template <>
std::vector<StorageOccurrence> SqliteIndexStorage::doGetAll<StorageOccurrence>(const std::string& query) const;
template <>
std::vector<StorageComponentAccess> SqliteIndexStorage::doGetAll<StorageComponentAccess>(const std::string& query) const;
template <>
std::vector<StorageCommentLocation> SqliteIndexStorage::doGetAll<StorageCommentLocation>(const std::string& query) const;
template <>
std::vector<StorageError> SqliteIndexStorage::doGetAll<StorageError>(const std::string& query) const;

#endif // SQLITE_INDEX_STORAGE_H
