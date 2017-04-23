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
	virtual Id getNodeIdForFileNode(const FilePath& filePath) const;
	virtual Id getNodeIdForNameHierarchy(const NameHierarchy& nameHierarchy) const;
	virtual std::vector<Id> getNodeIdsForNameHierarchies(const std::vector<NameHierarchy> nameHierarchies) const;

	virtual NameHierarchy getNameHierarchyForNodeId(Id id) const;
	virtual std::vector<NameHierarchy> getNameHierarchiesForNodeIds(const std::vector<Id> nodeIds) const;

	virtual Node::NodeType getNodeTypeForNodeWithId(Id id) const;
	virtual bool checkNodeExistsByName(const std::string& serializedName) const;

	virtual Id getIdForEdge(
		Edge::EdgeType type, const NameHierarchy& fromNameHierarchy, const NameHierarchy& toNameHierarchy) const;
	virtual StorageEdge getEdgeById(Id edgeId) const;
	virtual bool checkEdgeExists(Id edgeId) const;

	virtual std::shared_ptr<SourceLocationCollection> getFullTextSearchLocations(
			const std::string& searchTerm, bool caseSensitive) const;
	virtual std::vector<SearchMatch> getAutocompletionMatches(const std::string& query) const;
	virtual std::vector<SearchMatch> getSearchMatchesForTokenIds(const std::vector<Id>& tokenIds) const;

	virtual std::shared_ptr<Graph> getGraphForAll() const;
	virtual std::shared_ptr<Graph> getGraphForActiveTokenIds(const std::vector<Id>& tokenIds, bool* isActiveNamespace = nullptr) const;
	virtual std::shared_ptr<Graph> getGraphForTrail(Id originId, Id targetId, Edge::EdgeTypeMask trailType, size_t depth) const;

	virtual std::vector<Id> getActiveTokenIdsForId(Id tokenId, Id* declarationId) const;
	virtual std::vector<Id> getNodeIdsForLocationIds(const std::vector<Id>& locationIds) const;

	virtual std::shared_ptr<SourceLocationCollection> getSourceLocationsForTokenIds(
			const std::vector<Id>& tokenIds
	) const;
	virtual std::shared_ptr<SourceLocationCollection> getSourceLocationsForLocationIds(
			const std::vector<Id>& locationIds
	) const;
	virtual std::shared_ptr<SourceLocationFile> getSourceLocationsForFile(const FilePath& filePath) const;
	virtual std::shared_ptr<SourceLocationFile> getSourceLocationsForLinesInFile(
		const std::string& filePath, uint firstLineNumber, uint lastLineNumber
	) const;

	virtual std::shared_ptr<SourceLocationFile> getCommentLocationsInFile(const FilePath& filePath) const;

	virtual std::shared_ptr<TextAccess> getFileContent(const FilePath& filePath) const;

	virtual FileInfo getFileInfoForFilePath(const FilePath& filePath) const;
	virtual std::vector<FileInfo> getFileInfosForFilePaths(const std::vector<FilePath>& filePaths) const;

	virtual StorageStats getStorageStats() const;

	virtual ErrorCountInfo getErrorCount() const;
	virtual std::vector<ErrorInfo> getErrors() const;

	virtual std::shared_ptr<SourceLocationCollection> getErrorSourceLocations(std::vector<ErrorInfo>* errors) const;

	virtual Id addNodeBookmark(const NodeBookmark& bookmark);
	virtual Id addEdgeBookmark(const EdgeBookmark& bookmark);
	virtual Id addBookmarkCategory(const std::string& categoryName);

	virtual void updateBookmark(const Id bookmarkId, const std::string& name, const std::string& comment, const std::string& categoryName);

	virtual void removeBookmark(const Id id);
	virtual void removeBookmarkCategory(const Id id);

	virtual std::vector<NodeBookmark> getAllNodeBookmarks() const;
	virtual std::vector<EdgeBookmark> getAllEdgeBookmarks() const;

	virtual std::vector<BookmarkCategory> getAllBookmarkCategories() const;

protected:
	virtual void setErrorFilter(const ErrorFilter& filter);

private:
	void handleMessage(MessageErrorFilterChanged* message);

	StorageAccess* m_subject;
};

#endif // STORAGE_ACCESS_PROXY_H
