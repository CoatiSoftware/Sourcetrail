#ifndef STORAGE_ACCESS_H
#define STORAGE_ACCESS_H

#include <memory>
#include <string>
#include <vector>

#include "utility/types.h"

#include "data/bookmark/BookmarkCategory.h"
#include "data/bookmark/EdgeBookmark.h"
#include "data/bookmark/NodeBookmark.h"
#include "data/graph/Node.h"
#include "data/search/SearchMatch.h"
#include "data/storage/type/StorageEdge.h"
#include "data/ErrorCountInfo.h"
#include "data/ErrorFilter.h"
#include "data/ErrorInfo.h"
#include "data/storage/StorageStats.h"
#include "data/tooltip/TooltipInfo.h"
#include "data/tooltip/TooltipOrigin.h"

class FilePath;
class Graph;
class SourceLocationCollection;
class SourceLocationFile;
class TextAccess;

struct FileInfo;

class StorageAccess
{
public:
	virtual ~StorageAccess();

	virtual Id getNodeIdForFileNode(const FilePath& filePath) const = 0;
	virtual Id getNodeIdForNameHierarchy(const NameHierarchy& nameHierarchy) const = 0;
	virtual std::vector<Id> getNodeIdsForNameHierarchies(const std::vector<NameHierarchy> nameHierarchies) const = 0;

	virtual NameHierarchy getNameHierarchyForNodeId(Id id) const = 0;
	virtual std::vector<NameHierarchy> getNameHierarchiesForNodeIds(const std::vector<Id>& nodeIds) const = 0;

	virtual Node::NodeType getNodeTypeForNodeWithId(Id id) const = 0;

	virtual Id getIdForEdge(
		Edge::EdgeType type, const NameHierarchy& fromNameHierarchy, const NameHierarchy& toNameHierarchy) const = 0;
	virtual StorageEdge getEdgeById(Id edgeId) const = 0;

	virtual std::shared_ptr<SourceLocationCollection> getFullTextSearchLocations(
			const std::string& searchTerm, bool caseSensitive) const = 0;
	virtual std::vector<SearchMatch> getAutocompletionMatches(const std::string& query, Node::NodeTypeMask filter) const = 0;
	virtual std::vector<SearchMatch> getSearchMatchesForTokenIds(const std::vector<Id>& tokenIds) const = 0;

	virtual std::shared_ptr<Graph> getGraphForAll() const = 0;
	virtual std::shared_ptr<Graph> getGraphForFilter(Node::NodeTypeMask filter) const = 0;
	virtual std::shared_ptr<Graph> getGraphForActiveTokenIds(
		const std::vector<Id>& tokenIds, const std::vector<Id>& expandedNodeIds, bool* isActiveNamespace = nullptr) const = 0;
	virtual std::shared_ptr<Graph> getGraphForChildrenOfNodeId(Id nodeId) const = 0;
	virtual std::shared_ptr<Graph> getGraphForTrail(Id originId, Id targetId, Edge::EdgeTypeMask trailType, size_t depth) const = 0;

	virtual std::vector<Id> getActiveTokenIdsForId(Id tokenId, Id* declarationId) const = 0;
	virtual std::vector<Id> getNodeIdsForLocationIds(const std::vector<Id>& locationIds) const = 0;

	virtual std::shared_ptr<SourceLocationCollection> getSourceLocationsForTokenIds(
		const std::vector<Id>& tokenIds) const = 0;
	virtual std::shared_ptr<SourceLocationCollection> getSourceLocationsForLocationIds(
		const std::vector<Id>& locationIds) const = 0;
	virtual std::shared_ptr<SourceLocationFile> getSourceLocationsForFile(const FilePath& filePath) const = 0;
	virtual std::shared_ptr<SourceLocationFile> getSourceLocationsForLinesInFile(
		const FilePath& filePath, uint firstLineNumber, uint lastLineNumber) const = 0;

	virtual std::shared_ptr<SourceLocationFile> getCommentLocationsInFile(const FilePath& filePath) const = 0;

	virtual std::shared_ptr<TextAccess> getFileContent(const FilePath& filePath) const = 0;

	virtual FileInfo getFileInfoForFilePath(const FilePath& filePath) const = 0;
	virtual std::vector<FileInfo> getFileInfosForFilePaths(const std::vector<FilePath>& filePaths) const = 0;

	virtual StorageStats getStorageStats() const = 0;

	virtual ErrorCountInfo getErrorCount() const = 0;
	virtual std::vector<ErrorInfo> getErrorsLimited() const = 0;
	virtual std::shared_ptr<SourceLocationCollection> getErrorSourceLocationsLimited(std::vector<ErrorInfo>* errors) const = 0;

	virtual void setErrorFilter(const ErrorFilter& filter);

	// todo: remove bookmark related methods from storage access
	virtual Id addNodeBookmark(const NodeBookmark& bookmark) = 0;
	virtual Id addEdgeBookmark(const EdgeBookmark& bookmark) = 0;
	virtual Id addBookmarkCategory(const std::string& categoryName) = 0;

	virtual void updateBookmark(
		const Id bookmarkId, const std::string& name, const std::string& comment, const std::string& categoryName) = 0;
	virtual void removeBookmark(const Id id) = 0;
	virtual void removeBookmarkCategory(const Id id) = 0;

	virtual std::vector<NodeBookmark> getAllNodeBookmarks() const = 0;
	virtual std::vector<EdgeBookmark> getAllEdgeBookmarks() const = 0;
	virtual std::vector<BookmarkCategory> getAllBookmarkCategories() const = 0;

	virtual TooltipInfo getTooltipInfoForTokenIds(const std::vector<Id>& tokenIds, TooltipOrigin origin) const = 0;
	virtual TooltipInfo getTooltipInfoForSourceLocationIdsAndLocalSymbolIds(
		const std::vector<Id>& locationIds, const std::vector<Id>& localSymbolIds) const = 0;

protected:
	ErrorFilter m_errorFilter;
};

#endif // STORAGE_ACCESS_H
