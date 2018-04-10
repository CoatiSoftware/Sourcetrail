#ifndef STORAGE_ACCESS_PROXY_H
#define STORAGE_ACCESS_PROXY_H

#include "data/access/StorageAccess.h"

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageErrorFilterChanged.h"

class StorageAccessProxy
	: public StorageAccess
	, public MessageListener<MessageErrorFilterChanged>
{
public:
	StorageAccessProxy();
	virtual ~StorageAccessProxy();

	bool hasSubject() const;
	void setSubject(StorageAccess* subject);

	// StorageAccess implementation
	virtual Id getNodeIdForFileNode(const FilePath& filePath) const override;
	virtual Id getNodeIdForNameHierarchy(const NameHierarchy& nameHierarchy) const override;
	virtual std::vector<Id> getNodeIdsForNameHierarchies(const std::vector<NameHierarchy> nameHierarchies) const override;

	virtual NameHierarchy getNameHierarchyForNodeId(Id id) const override;
	virtual std::vector<NameHierarchy> getNameHierarchiesForNodeIds(const std::vector<Id>& nodeIds) const override;
	virtual std::map<Id, std::pair<Id, NameHierarchy>> getNodeIdToParentFileMap(const std::vector<Id>& nodeIds) const override;

	virtual NodeType getNodeTypeForNodeWithId(Id id) const override;

	virtual Id getIdForEdge(
		Edge::EdgeType type, const NameHierarchy& fromNameHierarchy, const NameHierarchy& toNameHierarchy) const override;
	virtual StorageEdge getEdgeById(Id edgeId) const override;

	virtual std::shared_ptr<SourceLocationCollection> getFullTextSearchLocations(
			const std::wstring& searchTerm, bool caseSensitive) const override;
	virtual std::vector<SearchMatch> getAutocompletionMatches(const std::wstring& query, NodeTypeSet acceptedNodeTypes) const override;
	virtual std::vector<SearchMatch> getSearchMatchesForTokenIds(const std::vector<Id>& tokenIds) const override;

	virtual std::shared_ptr<Graph> getGraphForAll() const override;
	virtual std::shared_ptr<Graph> getGraphForNodeTypes(NodeTypeSet nodeTypes) const override;
	virtual std::shared_ptr<Graph> getGraphForActiveTokenIds(
		const std::vector<Id>& tokenIds, const std::vector<Id>& expandedNodeIds, bool* isActiveNamespace = nullptr) const override;
	virtual std::shared_ptr<Graph> getGraphForChildrenOfNodeId(Id nodeId) const override;
	virtual std::shared_ptr<Graph> getGraphForTrail(Id originId, Id targetId, Edge::TypeMask trailType, size_t depth) const override;

	virtual std::vector<Id> getActiveTokenIdsForId(Id tokenId, Id* declarationId) const override;
	virtual std::vector<Id> getNodeIdsForLocationIds(const std::vector<Id>& locationIds) const override;

	virtual std::shared_ptr<SourceLocationCollection> getSourceLocationsForTokenIds(
			const std::vector<Id>& tokenIds
	) const override;
	virtual std::shared_ptr<SourceLocationCollection> getSourceLocationsForLocationIds(
			const std::vector<Id>& locationIds
	) const override;

	virtual std::shared_ptr<SourceLocationFile> getSourceLocationsForFile(const FilePath& filePath) const override;
	virtual std::shared_ptr<SourceLocationFile> getSourceLocationsForLinesInFile(
		const FilePath& filePath, size_t startLine, size_t endLine) const override;
	virtual std::shared_ptr<SourceLocationFile> getSourceLocationsOfTypeInFile(
		const FilePath& filePath, LocationType type) const override;

	virtual std::shared_ptr<SourceLocationFile> getCommentLocationsInFile(const FilePath& filePath) const override;

	virtual std::shared_ptr<TextAccess> getFileContent(const FilePath& filePath) const override;

	virtual FileInfo getFileInfoForFilePath(const FilePath& filePath) const override;
	virtual std::vector<FileInfo> getFileInfosForFilePaths(const std::vector<FilePath>& filePaths) const override;

	virtual StorageStats getStorageStats() const override;

	virtual ErrorCountInfo getErrorCount() const override;
	virtual std::vector<ErrorInfo> getErrorsLimited(const std::vector<Id>& errorIds) const override;
	virtual std::vector<Id> getErrorIdsForFile(const FilePath& filePath) const override;
	virtual std::shared_ptr<SourceLocationCollection> getErrorSourceLocations(
		const std::vector<ErrorInfo>& errors) const override;

	// TODO: remove these from access because it's not a getter!
	virtual Id addNodeBookmark(const NodeBookmark& bookmark) override;
	virtual Id addEdgeBookmark(const EdgeBookmark& bookmark) override;
	virtual Id addBookmarkCategory(const std::wstring& categoryName) override;

	virtual void updateBookmark(
		const Id bookmarkId, const std::wstring& name, const std::wstring& comment, const std::wstring& categoryName) override;
	virtual void removeBookmark(const Id id) override;
	virtual void removeBookmarkCategory(const Id id) override;
	// END TODO

	virtual std::vector<NodeBookmark> getAllNodeBookmarks() const override;
	virtual std::vector<EdgeBookmark> getAllEdgeBookmarks() const override;
	virtual std::vector<BookmarkCategory> getAllBookmarkCategories() const override;

	virtual TooltipInfo getTooltipInfoForTokenIds(const std::vector<Id>& tokenIds, TooltipOrigin origin) const override;
	virtual TooltipInfo getTooltipInfoForSourceLocationIdsAndLocalSymbolIds(
		const std::vector<Id>& locationIds, const std::vector<Id>& localSymbolIds) const override;

protected:
	virtual void setErrorFilter(const ErrorFilter& filter) override;

private:
	void handleMessage(MessageErrorFilterChanged* message) override;

	StorageAccess* m_subject;
};

#endif // STORAGE_ACCESS_PROXY_H
