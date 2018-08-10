#ifndef SQLITE_INDEX_STORAGE_H
#define SQLITE_INDEX_STORAGE_H

#include <memory>
#include <string>
#include <vector>

#include "data/location/LocationType.h"
#include "data/storage/sqlite/SqliteDatabaseIndex.h"
#include "data/storage/sqlite/SqliteStorage.h"
#include "data/storage/type/StorageCommentLocation.h"
#include "data/storage/type/StorageComponentAccess.h"
#include "data/storage/type/StorageEdge.h"
#include "data/storage/type/StorageError.h"
#include "data/storage/type/StorageFile.h"
#include "data/storage/type/StorageLocalSymbol.h"
#include "data/storage/type/StorageNode.h"
#include "data/storage/type/StorageOccurrence.h"
#include "data/storage/type/StorageSourceLocation.h"
#include "data/storage/type/StorageSymbol.h"
#include "utility/types.h"
#include "utility/utility.h"
#include "utility/utilityString.h"

class TextAccess;
class Version;
class SourceLocationCollection;
class SourceLocationFile;
struct ParseLocation;

class SqliteIndexStorage
	: public SqliteStorage
{
public:
	enum StorageModeType
	{
		STORAGE_MODE_READ = 1,
		STORAGE_MODE_WRITE = 2,
		STORAGE_MODE_CLEAR = 4,
	};

	SqliteIndexStorage(const FilePath& dbFilePath);

	virtual size_t getStaticVersion() const;

	void setMode(const StorageModeType mode);

	std::string getProjectSettingsText() const;
	void setProjectSettingsText(std::string text);

	StorageNode addNode(const StorageNodeData& data);
	void addSymbol(const StorageSymbol& data);
	void addFile(const StorageFile& data);
	StorageEdge addEdge(const StorageEdgeData& data);
	StorageLocalSymbol addLocalSymbol(const StorageLocalSymbolData& data);
	StorageSourceLocation addSourceLocation(const StorageSourceLocationData& data);
	bool addOccurrence(const StorageOccurrence& data);
	bool addOccurrences(const std::vector<StorageOccurrence>& occurrences);
	bool addComponentAccess(const StorageComponentAccess& componentAccess);
	StorageCommentLocation addCommentLocation(const StorageCommentLocationData& data);
	StorageError addError(const StorageErrorData& data);

	void removeElement(Id id);
	void removeElements(const std::vector<Id>& ids);
	void removeElementsWithLocationInFiles(const std::vector<Id>& fileIds, std::function<void(int)> updateStatusCallback);

	void removeAllErrors();
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

	StorageNode getNodeById(Id id) const;
	StorageNode getNodeBySerializedName(const std::wstring& serializedName) const;

	StorageFile getFileByPath(const std::wstring& filePath) const;

	std::vector<StorageFile> getFilesByPaths(const std::vector<FilePath>& filePaths) const;
	std::shared_ptr<TextAccess> getFileContentByPath(const std::wstring& filePath) const;
	std::shared_ptr<TextAccess> getFileContentById(Id fileId) const;

	void setFileIndexed(Id fileId, bool indexed);
	void setFileCompleteIfNoError(Id fileId, const std::wstring& filePath, bool complete);
	void setNodeType(int type, Id nodeId);

	std::shared_ptr<SourceLocationFile> getSourceLocationsForFile(
		const FilePath& filePath, const std::string& query = "") const;
	std::shared_ptr<SourceLocationFile> getSourceLocationsForLinesInFile(
		const FilePath& filePath, size_t startLine, size_t endLine) const;
	std::shared_ptr<SourceLocationFile> getSourceLocationsOfTypeInFile(
		const FilePath& filePath, LocationType type) const;

	std::shared_ptr<SourceLocationCollection> getSourceLocationsForElementIds(const std::vector<Id>& elementIds) const;

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
	int getErrorCount() const;

private:
	static const size_t s_storageVersion;

	std::vector<std::pair<int, SqliteDatabaseIndex>> getIndices() const;

	virtual void clearTables();
	virtual void setupTables();
	virtual void setupPrecompiledStatements();

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

	std::map<std::wstring, std::pair<Id, int>> m_tempNodeIndex;
	std::map<std::string, Id> m_tempEdgeIndex;
	std::map<std::wstring, Id> m_tempLocalSymbolIndex;
	std::map<Id, std::map<std::string, Id>> m_tempSourceLocationIndices;

	CppSQLite3Statement m_insertElementStmt;
	CppSQLite3Statement m_insertEdgeStmt;
	CppSQLite3Statement m_inserNodeStmt;
	CppSQLite3Statement m_insertSymbolStmt;
	CppSQLite3Statement m_insertFileStmt;
	CppSQLite3Statement m_insertFileContentStmt;
	CppSQLite3Statement m_inserLocalSymbolStmt;
	CppSQLite3Statement m_insertSourceLocationStmt;
	CppSQLite3Statement m_insertOccurrenceStmt;
	CppSQLite3Statement m_insert100OccurrencesStmt;
	CppSQLite3Statement m_insertComponentAccessStmt;
	CppSQLite3Statement m_checkCommentLocationExistsStmt;
	CppSQLite3Statement m_insertCommentLocationStmt;
	CppSQLite3Statement m_checkErrorExistsStmt;
	CppSQLite3Statement m_insertErrorStmt;
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
