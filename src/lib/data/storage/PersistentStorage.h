#ifndef PERSISTENT_STORAGE_H
#define PERSISTENT_STORAGE_H

#include <memory>
#include <vector>

#include "data/access/StorageAccess.h"
#include "data/fulltextsearch/FullTextSearchIndex.h"
#include "data/search/SearchIndex.h"
#include "data/HierarchyCache.h"
#include "data/storage/sqlite/SqliteIndexStorage.h"
#include "data/storage/sqlite/SqliteBookmarkStorage.h"
#include "data/storage/Storage.h"

class PersistentStorage
	: public Storage
	, public StorageAccess
{
public:
	PersistentStorage(const FilePath& dbPath, const FilePath& bookmarkPath);
	virtual ~PersistentStorage();

	virtual Id addNode(int type, const std::string& serializedName);
	virtual void addFile(const Id id, const std::string& filePath, const std::string& modificationTime, bool complete);
	virtual void addSymbol(const Id id, int definitionKind);
	virtual Id addEdge(int type, Id sourceId, Id targetId);
	virtual Id addLocalSymbol(const std::string& name);
	virtual Id addSourceLocation(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, int type);
	virtual void addOccurrence(Id elementId, Id sourceLocationId);
	virtual void addComponentAccess(Id nodeId , int type);
	virtual void addCommentLocation(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol);
	virtual void addError(const std::string& message, const std::string& commandline, const FilePath& filePath,
		uint startLine, uint startCol, bool fatal, bool indexed);

	virtual void forEachNode(std::function<void(const StorageNode& /*data*/)> callback) const;
	virtual void forEachFile(std::function<void(const StorageFile& /*data*/)> callback) const;
	virtual void forEachSymbol(std::function<void(const StorageSymbol& /*data*/)> callback) const;
	virtual void forEachEdge(std::function<void(const StorageEdge& /*data*/)> callback) const;
	virtual void forEachLocalSymbol(std::function<void(const StorageLocalSymbol& /*data*/)> callback) const;
	virtual void forEachSourceLocation(std::function<void(const StorageSourceLocation& /*data*/)> callback) const;
	virtual void forEachOccurrence(std::function<void(const StorageOccurrence& /*data*/)> callback) const;
	virtual void forEachComponentAccess(std::function<void(const StorageComponentAccess& /*data*/)> callback) const;
	virtual void forEachCommentLocation(std::function<void(const StorageCommentLocation& /*data*/)> callback) const;
	virtual void forEachError(std::function<void(const StorageError& /*data*/)> callback) const;

	virtual void startInjection();
	virtual void finishInjection();

	void setMode(const SqliteIndexStorage::StorageModeType mode);

	FilePath getDbFilePath() const;

	bool isEmpty() const;
	bool isIncompatible() const;
	std::string getProjectSettingsText() const;
	void setProjectSettingsText(std::string text);

	void setup();
	void clear();
	void clearCaches();

	std::set<FilePath> getReferenced(const std::set<FilePath>& filePaths);
	std::set<FilePath> getReferencing(const std::set<FilePath>& filePaths);

	void clearFileElements(const std::vector<FilePath>& filePaths, std::function<void(int)> updateStatusCallback);

	std::vector<FileInfo> getInfoOnAllFiles() const;

	void buildCaches();

	void optimizeMemory();

	// StorageAccess implementation
	virtual Id getNodeIdForFileNode(const FilePath& filePath) const;
	virtual Id getNodeIdForNameHierarchy(const NameHierarchy& nameHierarchy) const;
	virtual std::vector<Id> getNodeIdsForNameHierarchies(const std::vector<NameHierarchy> nameHierarchies) const;

	virtual NameHierarchy getNameHierarchyForNodeId(Id nodeId) const;
	virtual std::vector<NameHierarchy> getNameHierarchiesForNodeIds(const std::vector<Id>& nodeIds) const;

	virtual Node::NodeType getNodeTypeForNodeWithId(Id nodeId) const;

	virtual Id getIdForEdge(
		Edge::EdgeType type, const NameHierarchy& fromNameHierarchy, const NameHierarchy& toNameHierarchy) const;
	virtual StorageEdge getEdgeById(Id edgeId) const;

	virtual std::shared_ptr<SourceLocationCollection> getFullTextSearchLocations(
		const std::string& searchTerm, bool caseSensitive) const;

	virtual std::vector<SearchMatch> getAutocompletionMatches(const std::string& query) const;
	std::vector<SearchMatch> getAutocompletionSymbolMatches(const std::string& query, size_t maxResultsCount) const;
	std::vector<SearchMatch> getAutocompletionFileMatches(const std::string& query, size_t maxResultsCount) const;
	std::vector<SearchMatch> getAutocompletionCommandMatches(const std::string& query) const;
	virtual std::vector<SearchMatch> getSearchMatchesForTokenIds(const std::vector<Id>& elementIds) const;

	virtual std::shared_ptr<Graph> getGraphForAll() const;
	virtual std::shared_ptr<Graph> getGraphForActiveTokenIds(
		const std::vector<Id>& tokenIds, const std::vector<Id>& expandedNodeIds, bool* isActiveNamespace = nullptr) const;
	virtual std::shared_ptr<Graph> getGraphForChildrenOfNodeId(Id nodeId) const;
	virtual std::shared_ptr<Graph> getGraphForTrail(Id originId, Id targetId, Edge::EdgeTypeMask trailType, size_t depth) const;

	virtual std::vector<Id> getActiveTokenIdsForId(Id tokenId, Id* declarationId) const;
	virtual std::vector<Id> getNodeIdsForLocationIds(const std::vector<Id>& locationIds) const;

	virtual std::shared_ptr<SourceLocationCollection> getSourceLocationsForTokenIds(const std::vector<Id>& tokenIds) const;
	virtual std::shared_ptr<SourceLocationCollection> getSourceLocationsForLocationIds(const std::vector<Id>& locationIds) const;

	virtual std::shared_ptr<SourceLocationFile> getSourceLocationsForFile(const FilePath& filePath) const;
	virtual std::shared_ptr<SourceLocationFile> getSourceLocationsForLinesInFile(
		const FilePath& filePath, uint firstLineNumber, uint lastLineNumber
	) const;

	virtual std::shared_ptr<SourceLocationFile> getCommentLocationsInFile(const FilePath& filePath) const;

	virtual std::shared_ptr<TextAccess> getFileContent(const FilePath& filePath) const;

	virtual FileInfo getFileInfoForFilePath(const FilePath& filePath) const;
	virtual std::vector<FileInfo> getFileInfosForFilePaths(const std::vector<FilePath>& filePaths) const;

	virtual StorageStats getStorageStats() const;

	virtual ErrorCountInfo getErrorCount() const;
	virtual ErrorCountInfo getErrorCount(const std::vector<ErrorInfo>& errors) const;
	virtual std::vector<ErrorInfo> getErrors() const;
	virtual std::vector<ErrorInfo> getErrorsLimited() const;
	virtual std::shared_ptr<SourceLocationCollection> getErrorSourceLocationsLimited(std::vector<ErrorInfo>* errors) const;

	virtual Id addNodeBookmark(const NodeBookmark& bookmark);
	virtual Id addEdgeBookmark(const EdgeBookmark& bookmark);
	virtual Id addBookmarkCategory(const std::string& categoryName);

	virtual void updateBookmark(const Id bookmarkId, const std::string& name, const std::string& comment, const std::string& categoryName);
	virtual void removeBookmark(const Id id);
	virtual void removeBookmarkCategory(const Id id);

	virtual std::vector<NodeBookmark> getAllNodeBookmarks() const;
	virtual std::vector<EdgeBookmark> getAllEdgeBookmarks() const;
	virtual std::vector<BookmarkCategory> getAllBookmarkCategories() const;

	virtual TooltipInfo getTooltipInfoForTokenIds(const std::vector<Id>& tokenIds, TooltipOrigin origin) const;
	TooltipSnippet getTooltipSnippetForNode(const StorageNode& node) const;
	virtual TooltipInfo getTooltipInfoForSourceLocationIdsAndLocalSymbolIds(
		const std::vector<Id>& locationIds, const std::vector<Id>& localSymbolIds) const;

private:
	Id getFileNodeId(const FilePath& filePath) const;
	std::vector<Id> getFileNodeIds(const std::vector<FilePath>& filePaths) const;
	std::set<Id> getFileNodeIds(const std::set<FilePath>& filePaths) const;
	FilePath getFileNodePath(Id fileId) const;
	bool getFileNodeComplete(const FilePath& filePath) const;

	std::unordered_map<Id, std::set<Id>> getFileIdToIncludingFileIdMap() const;
	std::unordered_map<Id, std::set<Id>> getFileIdToImportingFileIdMap() const;
	std::set<Id> getReferenced(const std::set<Id>& filePaths, std::unordered_map<Id, std::set<Id>> idToReferencingIdMap) const;
	std::set<Id> getReferencing(const std::set<Id>& filePaths, std::unordered_map<Id, std::set<Id>> idToReferencingIdMap) const;

	std::set<FilePath> getReferencedByIncludes(const std::set<FilePath>& filePaths);
	std::set<FilePath> getReferencedByImports(const std::set<FilePath>& filePaths);

	std::set<FilePath> getReferencingByIncludes(const std::set<FilePath>& filePaths);
	std::set<FilePath> getReferencingByImports(const std::set<FilePath>& filePaths);

	void addNodesToGraph(const std::vector<Id>& nodeIds, Graph* graph, bool addChildCount) const;
	void addEdgesToGraph(const std::vector<Id>& edgeIds, Graph* graph) const;
	void addNodesWithParentsAndEdgesToGraph(
		const std::vector<Id>& nodeIds, const std::vector<Id>& edgeIds, Graph* graph) const;

	void addAggregationEdgesToGraph(const Id nodeId, const std::vector<StorageEdge>& edgesToAggregate, Graph* graph) const;
	void addComponentAccessToGraph(Graph* graph) const;

	void addCompleteFlagsToSourceLocationCollection(SourceLocationCollection* collection) const;
	void addInheritanceChainsToGraph(const std::vector<Id>& nodeIds, Graph* graph) const;

	void buildFilePathMaps();
	void buildSearchIndex();
	void buildFullTextSearchIndex() const;
	void buildHierarchyCache();

	size_t m_preInjectionErrorCount;

	SearchIndex m_commandIndex;
	SearchIndex m_symbolIndex;
	SearchIndex m_fileIndex;

	mutable FullTextSearchIndex m_fullTextSearchIndex;

	SqliteIndexStorage m_sqliteIndexStorage;
	SqliteBookmarkStorage m_sqliteBookmarkStorage;

	std::map<FilePath, Id> m_fileNodeIds;
	std::map<Id, FilePath> m_fileNodePaths;
	std::map<Id, bool> m_fileNodeComplete;

	std::map<Id, DefinitionKind> m_symbolDefinitionKinds;

	HierarchyCache m_hierarchyCache;
};

#endif // PERSISTENT_STORAGE_H
