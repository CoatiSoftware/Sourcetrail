#ifndef SQLITE_INDEX_STORAGE_H
#define SQLITE_INDEX_STORAGE_H

#include <memory>
#include <string>
#include <vector>

#include "ErrorInfo.h"
#include "LocationType.h"
#include "LowMemoryStringMap.h"
#include "SqliteDatabaseIndex.h"
#include "SqliteStorage.h"
#include "StorageComponentAccess.h"
#include "StorageEdge.h"
#include "StorageError.h"
#include "StorageElementComponent.h"
#include "StorageFile.h"
#include "StorageLocalSymbol.h"
#include "StorageNode.h"
#include "StorageOccurrence.h"
#include "StorageSourceLocation.h"
#include "StorageSymbol.h"
#include "types.h"
#include "utility.h"
#include "utilityString.h"

class TextAccess;
class Version;
class SourceLocationCollection;
class SourceLocationFile;

class SqliteIndexStorage
	: public SqliteStorage
{
public:
	static size_t getStorageVersion();

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

	Id addNode(const StorageNodeData& data);
	std::vector<Id> addNodes(const std::vector<StorageNode>& nodes);
	bool addSymbol(const StorageSymbol& data);
	bool addSymbols(const std::vector<StorageSymbol>& symbols);
	bool addFile(const StorageFile& data);
	Id addEdge(const StorageEdgeData& data);
	std::vector<Id> addEdges(const std::vector<StorageEdge>& edges);
	Id addLocalSymbol(const StorageLocalSymbolData& data);
	std::vector<Id> addLocalSymbols(const std::set<StorageLocalSymbol>& symbols);
	Id addSourceLocation(const StorageSourceLocationData& data);
	std::vector<Id> addSourceLocations(const std::vector<StorageSourceLocation>& locations);
	bool addOccurrence(const StorageOccurrence& data);
	bool addOccurrences(const std::vector<StorageOccurrence>& occurrences);
	bool addComponentAccess(const StorageComponentAccess& componentAccess);
	bool addComponentAccesses(const std::vector<StorageComponentAccess>& componentAccesses);
	void addElementComponent(const StorageElementComponent& component);
	void addElementComponents(const std::vector<StorageElementComponent>& components);
	StorageError addError(const StorageErrorData& data);

	void removeElement(Id id);
	void removeElements(const std::vector<Id>& ids);
	void removeElementsWithLocationInFiles(const std::vector<Id>& fileIds, std::function<void(int)> updateStatusCallback);

	void removeAllErrors();

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

	std::vector<int> getAvailableNodeTypes() const;
	std::vector<int> getAvailableEdgeTypes() const;

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

	StorageComponentAccess getComponentAccessByNodeId(Id nodeId) const;
	std::vector<StorageComponentAccess> getComponentAccessesByNodeIds(const std::vector<Id>& nodeIds) const;

	std::vector<StorageElementComponent> getElementComponentsByElementIds(const std::vector<Id>& elementIds) const;

	std::vector<ErrorInfo> getAllErrorInfos() const;

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

	template <typename StorageType>
	void forEach(std::function<void(StorageType&&)> func) const
	{
		forEach("", func);
	}

	template <typename StorageType>
	void forEachOfType(int type, std::function<void(StorageType&&)> func) const
	{
		forEach("WHERE type == " + std::to_string(type), func);
	}

	template <typename StorageType>
	void forEachByIds(const std::vector<Id> ids, std::function<void(StorageType&&)> func) const
	{
		if (ids.size())
		{
			forEach("WHERE id IN (" + utility::join(utility::toStrings(ids), ',') + ")", func);
		}
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

	struct TempSourceLocation
	{
		TempSourceLocation(uint32_t startLine, uint16_t lineDiff, uint16_t startCol, uint16_t endCol, uint8_t type)
			: startLine(startLine)
			, lineDiff(lineDiff)
			, startCol(startCol)
			, endCol(endCol)
			, type(type)
		{}

		bool operator<(const TempSourceLocation& other) const
		{
			if (startLine != other.startLine)
			{
				return startLine < other.startLine;
			}
			else if (lineDiff != other.lineDiff)
			{
				return lineDiff < other.lineDiff;
			}
			else if (startCol != other.startCol)
			{
				return startCol < other.startCol;
			}
			else if (endCol != other.endCol)
			{
				return endCol < other.endCol;
			}
			else
			{
				return type < other.type;
			}
		}

		uint32_t startLine;
		uint16_t lineDiff;
		uint16_t startCol;
		uint16_t endCol;
		uint8_t type;
	};

	std::vector<std::pair<int, SqliteDatabaseIndex>> getIndices() const;

	virtual void clearTables();
	virtual void setupTables();
	virtual void setupPrecompiledStatements();

	template <typename ResultType>
	std::vector<ResultType> doGetAll(const std::string& query) const
	{
		std::vector<ResultType> elements;
		forEach<ResultType>(
			query,
			[&elements](ResultType&& element)
			{
				elements.emplace_back(element);
			}
		);
		return elements;
	}

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

	template <typename StorageType>
	void forEach(const std::string& query, std::function<void(StorageType&&)> func) const;

	LowMemoryStringMap<std::string, uint32_t, 0> m_tempNodeNameIndex;
	LowMemoryStringMap<std::wstring, uint32_t, 0> m_tempWNodeNameIndex;
	std::map<uint32_t, int> m_tempNodeTypes;
	std::map<StorageEdgeData, uint32_t> m_tempEdgeIndex;
	std::map<std::wstring, std::map<std::wstring, uint32_t>> m_tempLocalSymbolIndex;
	std::map<uint32_t, std::map<TempSourceLocation, uint32_t>> m_tempSourceLocationIndices;

	template <typename StorageType>
	class InsertBatchStatement
	{
	public:
		void compile(
			const std::string header,
			size_t valueCount,
			std::function<void(CppSQLite3Statement& stmt, const StorageType&, size_t)> bindValuesFunc,
			CppSQLite3DB& database)
		{
			m_bindValuesFunc = bindValuesFunc;

			std::string valueStr = '(' + utility::join(std::vector<std::string>(valueCount, "?"), ',') + ')';

			const size_t MAX_VARIABLE_COUNT = 999;
			size_t batchSize = MAX_VARIABLE_COUNT / valueCount;

			while (true)
			{
				std::stringstream stmt;
				stmt << header;

				for (size_t i = 0; i < batchSize; i++)
				{
					if (i != 0)
					{
						stmt << ',';
					}
					stmt << valueStr;
				}
				stmt << ';';

				m_stmts.emplace_back(std::make_pair(batchSize, database.compileStatement(stmt.str().c_str())));

				if (batchSize == 1)
				{
					break;
				}
				else
				{
					batchSize /= 2;
				}
			}
		}

		bool execute(const std::vector<StorageType>& types, SqliteIndexStorage* storage)
		{
			size_t i = 0;
			for (std::pair<size_t, CppSQLite3Statement>& p : m_stmts)
			{
				const size_t& batchSize = p.first;
				CppSQLite3Statement& stmt = p.second;

				while (types.size() - i >= batchSize)
				{
					for (size_t j = 0; j < batchSize; j++)
					{
						m_bindValuesFunc(stmt, types[i + j], j);
					}

					const bool success = storage->executeStatement(stmt);
					if (!success)
					{
						return false;
					}

					i += batchSize;
				}
			}

			return true;
		}

	private:
		std::vector<std::pair<size_t, CppSQLite3Statement>> m_stmts;

		std::function<void(CppSQLite3Statement& stmt, const StorageType&, size_t)> m_bindValuesFunc;
	};

	InsertBatchStatement<StorageNode> m_insertNodeBatchStatement;
	InsertBatchStatement<StorageEdge> m_insertEdgeBatchStatement;
	InsertBatchStatement<StorageSymbol> m_insertSymbolBatchStatement;
	InsertBatchStatement<StorageLocalSymbol> m_insertLocalSymbolBatchStatement;
	InsertBatchStatement<StorageSourceLocationData> m_insertSourceLocationBatchStatement;
	InsertBatchStatement<StorageOccurrence> m_insertOccurenceBatchStatement;
	InsertBatchStatement<StorageComponentAccess> m_insertComponentAccessBatchStatement;

	CppSQLite3Statement m_insertElementStmt;
	CppSQLite3Statement m_insertElementComponentStmt;
	CppSQLite3Statement m_insertFileStmt;
	CppSQLite3Statement m_insertFileContentStmt;
	CppSQLite3Statement m_checkErrorExistsStmt;
	CppSQLite3Statement m_insertErrorStmt;
};

template <>
void SqliteIndexStorage::forEach<StorageEdge>(const std::string& query, std::function<void(StorageEdge&&)> func) const;
template <>
void SqliteIndexStorage::forEach<StorageNode>(const std::string& query, std::function<void(StorageNode&&)> func) const;
template <>
void SqliteIndexStorage::forEach<StorageSymbol>(const std::string& query, std::function<void(StorageSymbol&&)> func) const;
template <>
void SqliteIndexStorage::forEach<StorageFile>(const std::string& query, std::function<void(StorageFile&&)> func) const;
template <>
void SqliteIndexStorage::forEach<StorageLocalSymbol>(const std::string& query, std::function<void(StorageLocalSymbol&&)> func) const;
template <>
void SqliteIndexStorage::forEach<StorageSourceLocation>(const std::string& query, std::function<void(StorageSourceLocation&&)> func) const;
template <>
void SqliteIndexStorage::forEach<StorageOccurrence>(const std::string& query, std::function<void(StorageOccurrence&&)> func) const;
template <>
void SqliteIndexStorage::forEach<StorageComponentAccess>(const std::string& query, std::function<void(StorageComponentAccess&&)> func) const;
template <>
void SqliteIndexStorage::forEach<StorageElementComponent>(const std::string& query, std::function<void(StorageElementComponent&&)> func) const;
template <>
void SqliteIndexStorage::forEach<StorageError>(const std::string& query, std::function<void(StorageError&&)> func) const;

#endif // SQLITE_INDEX_STORAGE_H
