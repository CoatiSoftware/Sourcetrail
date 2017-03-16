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
	virtual Id getIdForNodeWithNameHierarchy(const NameHierarchy& nameHierarchy) const;
	virtual Id getIdForEdge(
		Edge::EdgeType type, const NameHierarchy& fromNameHierarchy, const NameHierarchy& toNameHierarchy) const;
	virtual StorageEdge getEdgeById(Id edgeId) const;

	virtual bool checkEdgeExists(Id edgeId) const;

	virtual NameHierarchy getNameHierarchyForNodeWithId(Id id) const;
	virtual Node::NodeType getNodeTypeForNodeWithId(Id id) const;
	virtual bool checkNodeExistsByName(const std::string& serializedName) const;

	virtual std::vector<NameHierarchy> getNameHierarchiesForNodeIds(const std::vector<Id> nodeIds) const;
	virtual std::vector<Id> getNodeIdsForNameHierarchies(const std::vector<NameHierarchy> nameHierarchies) const;

	virtual std::shared_ptr<TokenLocationCollection> getFullTextSearchLocations(
			const std::string& searchTerm, bool caseSensitive) const;
	virtual std::vector<SearchMatch> getAutocompletionMatches(const std::string& query) const;
	virtual std::vector<SearchMatch> getSearchMatchesForTokenIds(const std::vector<Id>& tokenIds) const;

	virtual std::shared_ptr<Graph> getGraphForAll() const;
	virtual std::shared_ptr<Graph> getGraphForActiveTokenIds(const std::vector<Id>& tokenIds, bool* isActiveNamespace = nullptr) const;

	virtual std::vector<Id> getActiveTokenIdsForId(Id tokenId, Id* declarationId) const;

	virtual std::vector<Id> getNodeIdsForLocationIds(const std::vector<Id>& locationIds) const;
	virtual std::vector<Id> getLocalSymbolIdsForLocationIds(const std::vector<Id>& locationIds) const;

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

	virtual Id addNodeBookmark(const NodeBookmark& bookmark);
	virtual Id addEdgeBookmark(const EdgeBookmark& bookmark);
	virtual Id addBookmarkCategory(const BookmarkCategory& category);

	virtual std::vector<NodeBookmark> getAllNodeBookmarks() const;
	virtual NodeBookmark getNodeBookmarkById(const Id bookmarkId) const;
	virtual bool checkNodeBookmarkExistsByTokens(const std::vector<std::string>& tokenNames) const;
	virtual void removeNodeBookmark(Id id);
	virtual void editNodeBookmark(const NodeBookmark& bookmark);

	virtual std::vector<EdgeBookmark> getAllEdgeBookmarks() const;
	virtual EdgeBookmark getEdgeBookmarkById(const Id bookmarkId) const;
	virtual bool checkEdgeBookmarkExistsByTokens(const std::vector<std::string>& tokenNames) const;
	virtual void removeEdgeBookmark(Id id);
	virtual void editEdgeBookmark(const EdgeBookmark& bookmark);

	virtual std::vector<BookmarkCategory> getAllBookmarkCategories() const;
	virtual bool checkBookmarkCategoryExists(const std::string& name) const;
	virtual void removeBookmarkCategory(Id id);

protected:
	virtual void setErrorFilter(const ErrorFilter& filter);

private:
	void handleMessage(MessageErrorFilterChanged* message);

	StorageAccess* m_subject;
};

#endif // STORAGE_ACCESS_PROXY_H
