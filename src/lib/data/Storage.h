#ifndef STORAGE_H
#define STORAGE_H

#include <memory>
#include <vector>

#include "utility/file/FilePath.h"

#include "data/access/StorageAccess.h"
#include "data/graph/token_component/TokenComponentAccess.h"
#include "data/location/TokenLocationCollection.h"
#include "data/parser/ParserClient.h"
#include "data/search/SearchIndex.h"
#include "data/HierarchyCache.h"
#include "data/SqliteStorage.h"

#include "data/parser/ParserClientImpl.h"

class Storage: public StorageAccess
{
public:
	Storage(const FilePath& dbPath);
	virtual ~Storage();

	FilePath getDbFilePath() const;
	Version getVersion() const;

	void init();
	void clear();
	void clearCaches();

	std::set<FilePath> getDependingFilePaths(const std::set<FilePath>& filePaths);
	std::set<FilePath> getDependingFilePaths(const FilePath& filePath);

	void clearFileElements(const std::vector<FilePath>& filePaths);
	void removeUnusedNames();

	std::vector<FileInfo> getInfoOnAllFiles() const;

	void logStats() const;

	void startParsing();
	void finishParsing();

	void injectData(std::shared_ptr<IntermediateStorage> injectedStorage);

	// StorageAccess implementation
	virtual Id getIdForNodeWithNameHierarchy(const NameHierarchy& nameHierarchy) const;
	virtual Id getIdForEdge(
		Edge::EdgeType type, const NameHierarchy& fromNameHierarchy, const NameHierarchy& toNameHierarchy) const;

	virtual Id getIdForFirstNode() const;

	virtual NameHierarchy getNameHierarchyForNodeWithId(Id nodeId) const;
	virtual Node::NodeType getNodeTypeForNodeWithId(Id nodeId) const;

	virtual std::vector<SearchMatch> getAutocompletionMatches(const std::string& query) const;
	virtual std::vector<SearchMatch> getSearchMatchesForTokenIds(const std::vector<Id>& elementIds) const;

	virtual std::shared_ptr<Graph> getGraphForAll() const;
	virtual std::shared_ptr<Graph> getGraphForActiveTokenIds(const std::vector<Id>& tokenIds) const;

	virtual std::vector<Id> getActiveTokenIdsForId(Id tokenId, Id* declarationId) const;

	virtual std::vector<Id> getNodeIdsForLocationIds(const std::vector<Id>& locationIds) const;
	virtual std::vector<Id> getLocalSymbolIdsForLocationIds(const std::vector<Id>& locationIds) const;

	virtual std::vector<Id> getTokenIdsForMatches(const std::vector<SearchMatch>& matches) const;
	virtual Id getTokenIdForFileNode(const FilePath& filePath) const;
	virtual std::vector<Id> getTokenIdsForAggregationEdge(Id sourceId, Id targetId) const;

	virtual std::shared_ptr<TokenLocationCollection> getTokenLocationsForTokenIds(const std::vector<Id>& tokenIds) const;
	virtual std::shared_ptr<TokenLocationCollection> getTokenLocationsForLocationIds(const std::vector<Id>& locationIds) const;
	virtual std::shared_ptr<TokenLocationFile> getTokenLocationsForFile(const std::string& filePath) const;
	virtual std::shared_ptr<TokenLocationFile> getTokenLocationsForLinesInFile(
		const std::string& filePath, uint firstLineNumber, uint lastLineNumber
	) const;

	virtual TokenLocationCollection getErrorTokenLocations(std::vector<ErrorInfo>* errors) const;
	virtual std::shared_ptr<TokenLocationFile> getCommentLocationsInFile(const FilePath& filePath) const;

	virtual std::shared_ptr<TextAccess> getFileContent(const FilePath& filePath) const;

	virtual FileInfo getFileInfoForFilePath(const FilePath& filePath) const;
	virtual std::vector<FileInfo> getFileInfosForFilePaths(const std::vector<FilePath>& filePaths) const;

	virtual ErrorCountInfo getErrorCount() const;
	virtual StorageStats getStorageStats() const;

private:
	Id getFileNodeId(const FilePath& filePath) const;
	FilePath getFileNodePath(Id fileId) const;

	Id getLastVisibleParentNodeId(const Id nodeId) const;
	std::vector<Id> getAllChildNodeIds(const Id nodeId) const;

	void addNodesToGraph(const std::vector<Id>& nodeIds, Graph* graph) const;
	void addEdgesToGraph(const std::vector<Id>& edgeIds, Graph* graph) const;
	void addNodesWithChildrenAndEdgesToGraph(const std::vector<Id>& nodeIds, const std::vector<Id>& edgeIds, Graph* graph) const;

	void addAggregationEdgesToGraph(const Id nodeId, Graph* graph) const;
	void addComponentAccessToGraph(Graph* graph) const;

	void buildSearchIndex();
	void buildHierarchyCache();

	void log(std::string type, std::string str, const ParseLocation& location) const;

	SearchIndex m_commandIndex;
	SearchIndex m_elementIndex;

	SqliteStorage m_sqliteStorage;

	mutable std::map <FilePath, Id> m_fileNodeIds;
	HierarchyCache m_hierarchyCache;
};

#endif // STORAGE_H
