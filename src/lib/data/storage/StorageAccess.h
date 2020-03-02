#ifndef STORAGE_ACCESS_H
#define STORAGE_ACCESS_H

#include <memory>
#include <string>
#include <vector>

#include "types.h"

#include "BookmarkCategory.h"
#include "EdgeBookmark.h"
#include "ErrorCountInfo.h"
#include "ErrorFilter.h"
#include "ErrorInfo.h"
#include "LocationType.h"
#include "Node.h"
#include "NodeBookmark.h"
#include "SearchMatch.h"
#include "StorageEdge.h"
#include "StorageStats.h"
#include "TooltipInfo.h"
#include "TooltipOrigin.h"

class FilePath;
class Graph;
class NodeTypeSet;
class SourceLocationCollection;
class SourceLocationFile;
class TextAccess;

struct FileInfo;

class StorageAccess
{
public:
	virtual ~StorageAccess() = default;

	virtual Id getNodeIdForFileNode(const FilePath& filePath) const = 0;
	virtual Id getNodeIdForNameHierarchy(const NameHierarchy& nameHierarchy) const = 0;
	virtual std::vector<Id> getNodeIdsForNameHierarchies(
		const std::vector<NameHierarchy> nameHierarchies) const = 0;

	virtual NameHierarchy getNameHierarchyForNodeId(Id id) const = 0;
	virtual std::vector<NameHierarchy> getNameHierarchiesForNodeIds(
		const std::vector<Id>& nodeIds) const = 0;
	virtual std::map<Id, std::pair<Id, NameHierarchy>> getNodeIdToParentFileMap(
		const std::vector<Id>& nodeIds) const = 0;

	virtual NodeType getNodeTypeForNodeWithId(Id id) const = 0;

	virtual StorageEdge getEdgeById(Id edgeId) const = 0;

	virtual std::shared_ptr<SourceLocationCollection> getFullTextSearchLocations(
		const std::wstring& searchTerm, bool caseSensitive) const = 0;
	virtual std::vector<SearchMatch> getAutocompletionMatches(
		const std::wstring& query, NodeTypeSet acceptedNodeTypes, bool acceptCommands) const = 0;
	virtual std::vector<SearchMatch> getSearchMatchesForTokenIds(
		const std::vector<Id>& tokenIds) const = 0;

	virtual std::shared_ptr<Graph> getGraphForAll() const = 0;
	virtual std::shared_ptr<Graph> getGraphForNodeTypes(NodeTypeSet nodeTypes) const = 0;
	virtual std::shared_ptr<Graph> getGraphForActiveTokenIds(
		const std::vector<Id>& tokenIds,
		const std::vector<Id>& expandedNodeIds,
		bool* isActiveNamespace = nullptr) const = 0;
	virtual std::shared_ptr<Graph> getGraphForChildrenOfNodeId(Id nodeId) const = 0;
	virtual std::shared_ptr<Graph> getGraphForTrail(
		Id originId,
		Id targetId,
		NodeKindMask nodeTypes,
		Edge::TypeMask edgeTypes,
		bool nodeNonIndexed,
		size_t depth,
		bool directed) const = 0;

	virtual NodeKindMask getAvailableNodeTypes() const = 0;
	virtual Edge::TypeMask getAvailableEdgeTypes() const = 0;

	virtual std::vector<Id> getActiveTokenIdsForId(Id tokenId, Id* declarationId) const = 0;
	virtual std::vector<Id> getNodeIdsForLocationIds(const std::vector<Id>& locationIds) const = 0;

	virtual std::shared_ptr<SourceLocationCollection> getSourceLocationsForTokenIds(
		const std::vector<Id>& tokenIds) const = 0;
	virtual std::shared_ptr<SourceLocationCollection> getSourceLocationsForLocationIds(
		const std::vector<Id>& locationIds) const = 0;

	virtual std::shared_ptr<SourceLocationFile> getSourceLocationsForFile(
		const FilePath& filePath) const = 0;
	virtual std::shared_ptr<SourceLocationFile> getSourceLocationsForLinesInFile(
		const FilePath& filePath, size_t startLine, size_t endLine) const = 0;
	virtual std::shared_ptr<SourceLocationFile> getSourceLocationsOfTypeInFile(
		const FilePath& filePath, LocationType type) const = 0;

	virtual std::shared_ptr<TextAccess> getFileContent(
		const FilePath& filePath, bool showsErrors) const = 0;

	virtual FileInfo getFileInfoForFileId(Id id) const = 0;

	virtual FileInfo getFileInfoForFilePath(const FilePath& filePath) const = 0;
	virtual std::vector<FileInfo> getFileInfosForFilePaths(
		const std::vector<FilePath>& filePaths) const = 0;

	virtual StorageStats getStorageStats() const = 0;

	virtual ErrorCountInfo getErrorCount() const = 0;
	virtual std::vector<ErrorInfo> getErrorsLimited(const ErrorFilter& filter) const = 0;
	virtual std::vector<ErrorInfo> getErrorsForFileLimited(
		const ErrorFilter& filter, const FilePath& filePath) const = 0;
	virtual std::shared_ptr<SourceLocationCollection> getErrorSourceLocations(
		const std::vector<ErrorInfo>& errors) const = 0;

	// todo: remove bookmark related methods from storage access
	virtual Id addNodeBookmark(const NodeBookmark& bookmark) = 0;
	virtual Id addEdgeBookmark(const EdgeBookmark& bookmark) = 0;
	virtual Id addBookmarkCategory(const std::wstring& categoryName) = 0;

	virtual void updateBookmark(
		const Id bookmarkId,
		const std::wstring& name,
		const std::wstring& comment,
		const std::wstring& categoryName) = 0;
	virtual void removeBookmark(const Id id) = 0;
	virtual void removeBookmarkCategory(const Id id) = 0;

	virtual std::vector<NodeBookmark> getAllNodeBookmarks() const = 0;
	virtual std::vector<EdgeBookmark> getAllEdgeBookmarks() const = 0;
	virtual std::vector<BookmarkCategory> getAllBookmarkCategories() const = 0;

	virtual TooltipInfo getTooltipInfoForTokenIds(
		const std::vector<Id>& tokenIds, TooltipOrigin origin) const = 0;
	virtual TooltipInfo getTooltipInfoForSourceLocationIdsAndLocalSymbolIds(
		const std::vector<Id>& locationIds, const std::vector<Id>& localSymbolIds) const = 0;

	virtual void setUseErrorCache(bool enabled) {}
	virtual void addErrorsToCache(const std::vector<ErrorInfo>& newErrors, const ErrorCountInfo& errorCount)
	{
	}
};

#endif	  // STORAGE_ACCESS_H
