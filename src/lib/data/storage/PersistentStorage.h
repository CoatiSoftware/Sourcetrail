#ifndef PERSISTENT_STORAGE_H
#define PERSISTENT_STORAGE_H

#include <memory>
#include <vector>

#include "StorageAccess.h"
#include "FullTextSearchIndex.h"
#include "SearchIndex.h"
#include "HierarchyCache.h"
#include "SqliteIndexStorage.h"
#include "SqliteBookmarkStorage.h"
#include "Storage.h"

class PersistentStorage
	: public Storage
	, public StorageAccess
{
public:
	PersistentStorage(const FilePath& dbPath, const FilePath& bookmarkPath);

	Id addNode(const StorageNodeData& data) override;
	void addSymbol(const StorageSymbol& data) override;
	void addFile(const StorageFile& data) override;
	Id addEdge(const StorageEdgeData& data) override;
	Id addLocalSymbol(const StorageLocalSymbolData& data) override;
	Id addSourceLocation(const StorageSourceLocationData& data) override;
	void addOccurrence(const StorageOccurrence& data) override;
	void addOccurrences(const std::vector<StorageOccurrence>& occurrences) override;
	void addComponentAccess(const StorageComponentAccess& componentAccess) override;
	void addCommentLocation(const StorageCommentLocationData& data) override;
	void addError(const StorageErrorData& data) override;

	void forEachNode(std::function<void(const StorageNode& /*data*/)> callback) const override;
	void forEachFile(std::function<void(const StorageFile& /*data*/)> callback) const override;
	void forEachSymbol(std::function<void(const StorageSymbol& /*data*/)> callback) const override;
	void forEachEdge(std::function<void(const StorageEdge& /*data*/)> callback) const override;
	void forEachLocalSymbol(std::function<void(const StorageLocalSymbol& /*data*/)> callback) const override;
	void forEachSourceLocation(std::function<void(const StorageSourceLocation& /*data*/)> callback) const override;
	void forEachOccurrence(std::function<void(const StorageOccurrence& /*data*/)> callback) const override;
	void forEachComponentAccess(std::function<void(const StorageComponentAccess& /*data*/)> callback) const override;
	void forEachCommentLocation(std::function<void(const StorageCommentLocationData& /*data*/)> callback) const override;
	void forEachError(std::function<void(const StorageErrorData& /*data*/)> callback) const override;

	void startInjection() override;
	void finishInjection() override;

	void setMode(const SqliteIndexStorage::StorageModeType mode);

	FilePath getIndexDbFilePath() const;
	FilePath getBookmarkDbFilePath() const;

	bool isEmpty() const;
	bool isIncompatible() const;
	std::string getProjectSettingsText() const;
	void setProjectSettingsText(std::string text);

	void setup();
	void updateVersion();
	void clear();
	void clearCaches();

	std::set<FilePath> getReferenced(const std::set<FilePath>& filePaths) const;
	std::set<FilePath> getReferencing(const std::set<FilePath>& filePaths) const;

	void clearAllErrors();
	void clearFileElements(const std::vector<FilePath>& filePaths, std::function<void(int)> updateStatusCallback);

	std::vector<FileInfo> getFileInfoForAllFiles() const;
	std::set<FilePath> getIncompleteFiles() const;
	bool getFilePathIndexed(const FilePath& path) const;

	void buildCaches();

	void optimizeMemory();

	// StorageAccess implementation
	Id getNodeIdForFileNode(const FilePath& filePath) const override;
	Id getNodeIdForNameHierarchy(const NameHierarchy& nameHierarchy) const override;
	std::vector<Id> getNodeIdsForNameHierarchies(const std::vector<NameHierarchy> nameHierarchies) const override;

	NameHierarchy getNameHierarchyForNodeId(Id nodeId) const override;
	std::vector<NameHierarchy> getNameHierarchiesForNodeIds(const std::vector<Id>& nodeIds) const override;
	std::map<Id, std::pair<Id, NameHierarchy>> getNodeIdToParentFileMap(const std::vector<Id>& nodeIds) const override;

	NodeType getNodeTypeForNodeWithId(Id nodeId) const override;

	StorageEdge getEdgeById(Id edgeId) const override;

	std::shared_ptr<SourceLocationCollection> getFullTextSearchLocations(
		const std::wstring& searchTerm, bool caseSensitive) const override;

	std::vector<SearchMatch> getAutocompletionMatches(const std::wstring& query, NodeTypeSet acceptedNodeTypes) const override;
	std::vector<SearchMatch> getAutocompletionSymbolMatches(
		const std::wstring& query, const NodeTypeSet& acceptedNodeTypes, size_t maxResultsCount, size_t maxBestScoredResultsLength) const;
	std::vector<SearchMatch> getAutocompletionFileMatches(const std::wstring& query, size_t maxResultsCount) const;
	std::vector<SearchMatch> getAutocompletionCommandMatches(const std::wstring& query, NodeTypeSet acceptedNodeTypes) const;
	std::vector<SearchMatch> getSearchMatchesForTokenIds(const std::vector<Id>& elementIds) const override;

	std::shared_ptr<Graph> getGraphForAll() const override;
	std::shared_ptr<Graph> getGraphForNodeTypes(NodeTypeSet nodeTypes) const override;
	std::shared_ptr<Graph> getGraphForActiveTokenIds(
		const std::vector<Id>& tokenIds, const std::vector<Id>& expandedNodeIds, bool* isActiveNamespace = nullptr) const override;
	std::shared_ptr<Graph> getGraphForChildrenOfNodeId(Id nodeId) const override;
	std::shared_ptr<Graph> getGraphForTrail(Id originId, Id targetId, Edge::TypeMask trailType, size_t depth) const override;

	std::vector<Id> getActiveTokenIdsForId(Id tokenId, Id* declarationId) const override;
	std::vector<Id> getNodeIdsForLocationIds(const std::vector<Id>& locationIds) const override;

	std::shared_ptr<SourceLocationCollection> getSourceLocationsForTokenIds(const std::vector<Id>& tokenIds) const override;
	std::shared_ptr<SourceLocationCollection> getSourceLocationsForLocationIds(const std::vector<Id>& locationIds) const override;

	std::shared_ptr<SourceLocationFile> getSourceLocationsForFile(const FilePath& filePath) const override;
	std::shared_ptr<SourceLocationFile> getSourceLocationsForLinesInFile(
		const FilePath& filePath, size_t startLine, size_t endLine) const override;
	std::shared_ptr<SourceLocationFile> getSourceLocationsOfTypeInFile(
		const FilePath& filePath, LocationType type) const override;

	std::shared_ptr<SourceLocationFile> getCommentLocationsInFile(const FilePath& filePath) const override;

	std::shared_ptr<TextAccess> getFileContent(const FilePath& filePath) const override;
	bool hasContentForFile(const FilePath& filePath) const;

	FileInfo getFileInfoForFileId(Id id) const override;

	FileInfo getFileInfoForFilePath(const FilePath& filePath) const override;
	std::vector<FileInfo> getFileInfosForFilePaths(const std::vector<FilePath>& filePaths) const override;

	StorageStats getStorageStats() const override;

	ErrorCountInfo getErrorCount() const override;
	std::vector<ErrorInfo> getErrorsLimited(const ErrorFilter& filter) const override;
	std::vector<ErrorInfo> getErrorsForFileLimited(
		const ErrorFilter& filter, const FilePath& filePath) const override;
	std::shared_ptr<SourceLocationCollection> getErrorSourceLocations(
		const std::vector<ErrorInfo>& errors) const override;

	Id addNodeBookmark(const NodeBookmark& bookmark) override;
	Id addEdgeBookmark(const EdgeBookmark& bookmark) override;
	Id addBookmarkCategory(const std::wstring& categoryName) override;

	void updateBookmark(const Id bookmarkId, const std::wstring& name, const std::wstring& comment, const std::wstring& categoryName) override;
	void removeBookmark(const Id id) override;
	void removeBookmarkCategory(const Id id) override;

	std::vector<NodeBookmark> getAllNodeBookmarks() const override;
	std::vector<EdgeBookmark> getAllEdgeBookmarks() const override;
	std::vector<BookmarkCategory> getAllBookmarkCategories() const override;

	TooltipInfo getTooltipInfoForTokenIds(const std::vector<Id>& tokenIds, TooltipOrigin origin) const override;
	TooltipSnippet getTooltipSnippetForNode(const StorageNode& node) const;
	TooltipInfo getTooltipInfoForSourceLocationIdsAndLocalSymbolIds(
		const std::vector<Id>& locationIds, const std::vector<Id>& localSymbolIds) const override;

private:
	Id getFileNodeId(const FilePath& filePath) const;
	std::vector<Id> getFileNodeIds(const std::vector<FilePath>& filePaths) const;
	std::set<Id> getFileNodeIds(const std::set<FilePath>& filePaths) const;
	FilePath getFileNodePath(Id fileId) const;
	bool getFileNodeComplete(Id fileId) const;
	bool getFileNodeIndexed(Id fileId) const;

	std::unordered_map<Id, std::set<Id>> getFileIdToIncludingFileIdMap() const;
	std::unordered_map<Id, std::set<Id>> getFileIdToIncludedFileIdMap() const;
	std::unordered_map<Id, std::set<Id>> getFileIdToImportingFileIdMap() const;
	std::set<Id> getReferenced(const std::set<Id>& filePaths, std::unordered_map<Id, std::set<Id>> idToReferencingIdMap) const;
	std::set<Id> getReferencing(const std::set<Id>& filePaths, std::unordered_map<Id, std::set<Id>> idToReferencingIdMap) const;

	std::set<FilePath> getReferencedByIncludes(const std::set<FilePath>& filePaths) const;
	std::set<FilePath> getReferencedByImports(const std::set<FilePath>& filePaths) const;

	std::set<FilePath> getReferencingByIncludes(const std::set<FilePath>& filePaths) const;
	std::set<FilePath> getReferencingByImports(const std::set<FilePath>& filePaths) const;

	void addNodesToGraph(const std::vector<Id>& nodeIds, Graph* graph, bool addChildCount) const;
	void addEdgesToGraph(const std::vector<Id>& edgeIds, Graph* graph) const;
	void addNodesWithParentsAndEdgesToGraph(
		const std::vector<Id>& nodeIds, const std::vector<Id>& edgeIds, Graph* graphh, bool addChildCount) const;

	void addAggregationEdgesToGraph(Id nodeId, const std::vector<StorageEdge>& edgesToAggregate, Graph* graph) const;
	void addFileContentsToGraph(Id fileId, Graph* graph) const;
	void addComponentAccessToGraph(Graph* graph) const;

	void addCompleteFlagsToSourceLocationCollection(SourceLocationCollection* collection) const;
	void addInheritanceChainsToGraph(const std::vector<Id>& nodeIds, Graph* graph) const;

	void buildFilePathMaps();
	void buildSearchIndex();
	void buildFullTextSearchIndex() const;
	void buildMemberEdgeIdOrderMap();
	void buildHierarchyCache();

	size_t m_preInjectionErrorCount = 0;

	SearchIndex m_commandIndex;
	SearchIndex m_symbolIndex;
	SearchIndex m_fileIndex;

	mutable FullTextSearchIndex m_fullTextSearchIndex;
	mutable std::string m_fullTextSearchCodec;

	SqliteIndexStorage m_sqliteIndexStorage;
	SqliteBookmarkStorage m_sqliteBookmarkStorage;

	std::map<FilePath, Id> m_fileNodeIds;
	std::map<FilePath, Id> m_lowerCasefileNodeIds;
	std::map<Id, FilePath> m_fileNodePaths;
	std::map<Id, bool> m_fileNodeComplete;
	std::map<Id, bool> m_fileNodeIndexed;

	std::map<Id, DefinitionKind> m_symbolDefinitionKinds;
	std::map<Id, Id> m_memberEdgeIdOrderMap;

	HierarchyCache m_hierarchyCache;

	bool m_hasJavaFiles = false;
};

#endif // PERSISTENT_STORAGE_H
