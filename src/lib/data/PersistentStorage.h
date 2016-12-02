#ifndef PERSISTENT_STORAGE_H
#define PERSISTENT_STORAGE_H

#include <memory>
#include <vector>

#include "utility/file/FilePath.h"

#include "data/access/StorageAccess.h"
#include "data/fulltextsearch/FullTextSearchIndex.h"
#include "data/graph/token_component/TokenComponentAccess.h"
#include "data/location/TokenLocationCollection.h"
#include "data/parser/ParserClient.h"
#include "data/parser/ParseLocation.h"
#include "data/search/SearchIndex.h"
#include "data/HierarchyCache.h"
#include "data/SqliteStorage.h"
#include "data/Storage.h"

#include "data/parser/ParserClientImpl.h"

class PersistentStorage
	: public Storage
	, public StorageAccess
{
public:
	PersistentStorage(const FilePath& dbPath);
	virtual ~PersistentStorage();

	virtual Id addFile(const std::string& name, const std::string& filePath, const std::string& modificationTime);
	virtual Id addNode(int type, const std::string& serializedName, int definitionType);
	virtual Id addEdge(int type, Id sourceId, Id targetId);
	virtual Id addLocalSymbol(const std::string& name);
	virtual Id addSourceLocation(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, int type);
	virtual void addOccurrence(Id elementId, Id sourceLocationId);
	virtual void addComponentAccess(Id nodeId , int type);
	virtual void addCommentLocation(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol);
	virtual void addError(const std::string& message, const FilePath& filePath, uint startLine, uint startCol, bool fatal, bool indexed);

	virtual void forEachFile(std::function<void(const Id /*id*/, const StorageFile& /*data*/)> callback) const;
	virtual void forEachNode(std::function<void(const Id /*id*/, const StorageNode& /*data*/)> callback) const;
	virtual void forEachEdge(std::function<void(const Id /*id*/, const StorageEdge& /*data*/)> callback) const;
	virtual void forEachLocalSymbol(std::function<void(const Id /*id*/, const StorageLocalSymbol& /*data*/)> callback) const;
	virtual void forEachSourceLocation(std::function<void(const Id /*id*/, const StorageSourceLocation& /*data*/)> callback) const;
	virtual void forEachOccurrence(std::function<void(const StorageOccurrence& /*data*/)> callback) const;
	virtual void forEachComponentAccess(std::function<void(const StorageComponentAccess& /*data*/)> callback) const;
	virtual void forEachCommentLocation(std::function<void(const StorageCommentLocation& /*data*/)> callback) const;
	virtual void forEachError(std::function<void(const StorageError& /*data*/)> callback) const;

	virtual void startInjection();
	virtual void finishInjection();

	void setMode(const SqliteStorage::StorageModeType mode);

	FilePath getDbFilePath() const;

	bool isEmpty() const;
	bool isIncompatible() const;
	std::string getProjectSettingsText() const;
	void setProjectSettingsText(std::string text);

	void setup();
	void clear();
	void clearCaches();

	std::set<FilePath> getDependingFilePaths(const std::set<FilePath>& filePaths);

	void clearFileElements(const std::vector<FilePath>& filePaths);

	std::vector<FileInfo> getInfoOnAllFiles() const;

	void buildCaches();

	void optimizeMemory();

	// StorageAccess implementation
	virtual Id getIdForNodeWithNameHierarchy(const NameHierarchy& nameHierarchy) const;
	virtual Id getIdForEdge(
		Edge::EdgeType type, const NameHierarchy& fromNameHierarchy, const NameHierarchy& toNameHierarchy) const;

	virtual NameHierarchy getNameHierarchyForNodeWithId(Id nodeId) const;
	virtual Node::NodeType getNodeTypeForNodeWithId(Id nodeId) const;

	virtual std::shared_ptr<TokenLocationCollection> getFullTextSearchLocations(
			const std::string& searchTerm, bool caseSensitive) const;
	virtual std::vector<SearchMatch> getAutocompletionMatches(const std::string& query) const;
	virtual std::vector<SearchMatch> getSearchMatchesForTokenIds(const std::vector<Id>& elementIds) const;

	virtual std::shared_ptr<Graph> getGraphForAll() const;
	virtual std::shared_ptr<Graph> getGraphForActiveTokenIds(const std::vector<Id>& tokenIds) const;

	virtual std::vector<Id> getActiveTokenIdsForId(Id tokenId, Id* declarationId) const;

	virtual std::vector<Id> getNodeIdsForLocationIds(const std::vector<Id>& locationIds) const;
	virtual std::vector<Id> getLocalSymbolIdsForLocationIds(const std::vector<Id>& locationIds) const;

	virtual std::vector<Id> getTokenIdsForMatches(const std::vector<SearchMatch>& matches) const;
	virtual Id getTokenIdForFileNode(const FilePath& filePath) const;

	virtual std::shared_ptr<TokenLocationCollection> getTokenLocationsForTokenIds(
			const std::vector<Id>& tokenIds
	) const;
	virtual std::shared_ptr<TokenLocationCollection> getTokenLocationsForLocationIds(
			const std::vector<Id>& locationIds
	) const;
	virtual std::shared_ptr<TokenLocationFile> getTokenLocationsForFile(const std::string& filePath) const;
	virtual std::shared_ptr<TokenLocationFile> getTokenLocationsForLinesInFile(
		const std::string& filePath, uint firstLineNumber, uint lastLineNumber
	) const;

	virtual std::shared_ptr<TokenLocationFile> getCommentLocationsInFile(const FilePath& filePath) const;

	virtual std::shared_ptr<TextAccess> getFileContent(const FilePath& filePath) const;

	virtual FileInfo getFileInfoForFilePath(const FilePath& filePath) const;
	virtual std::vector<FileInfo> getFileInfosForFilePaths(const std::vector<FilePath>& filePaths) const;

	virtual StorageStats getStorageStats() const;

	virtual ErrorCountInfo getErrorCount() const;
	virtual std::vector<ErrorInfo> getErrors() const;

	virtual std::shared_ptr<TokenLocationCollection> getErrorTokenLocations(std::vector<ErrorInfo>* errors) const;

private:
	Id getFileNodeId(const FilePath& filePath) const;
	FilePath getFileNodePath(Id fileId) const;

	std::set<FilePath> getDependingFilePathsForIncludes(const std::set<FilePath>& filePaths);
	std::set<FilePath> getDependingFilePathsForImports(const std::set<FilePath>& filePaths);

	Id getLastVisibleParentNodeId(const Id nodeId) const;
	std::vector<Id> getAllChildNodeIds(const Id nodeId) const;

	void addNodesToGraph(const std::vector<Id>& nodeIds, Graph* graph) const;
	void addEdgesToGraph(const std::vector<Id>& edgeIds, Graph* graph) const;
	void addNodesWithChildrenAndEdgesToGraph(
			const std::vector<Id>& nodeIds,
			const std::vector<Id>& edgeIds, Graph* graph
	) const;

	void addAggregationEdgesToGraph(const Id nodeId, Graph* graph) const;
	void addComponentAccessToGraph(Graph* graph) const;

	void buildSearchIndex();
	void buildFilePathMaps();
	void buildFullTextSearchIndex() const;
	void buildHierarchyCache();

	void log(std::string type, std::string str, const ParseLocation& location) const;

	size_t m_preInjectionErrorCount;

	SearchIndex m_commandIndex;
	SearchIndex m_elementIndex;
	SearchIndex m_fileIndex;

	mutable FullTextSearchIndex m_fullTextSearchIndex;

	SqliteStorage m_sqliteStorage;

	mutable std::map <FilePath, Id> m_fileNodeIds;
	mutable std::map <Id, FilePath> m_fileNodePaths;

	HierarchyCache m_hierarchyCache;
};

#endif // PERSISTENT_STORAGE_H
