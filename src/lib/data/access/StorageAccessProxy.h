#ifndef STORAGE_ACCESS_PROXY_H
#define STORAGE_ACCESS_PROXY_H

#include "data/access/StorageAccess.h"

#include "data/ErrorFilter.h"

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

	virtual NameHierarchy getNameHierarchyForNodeWithId(Id id) const;
	virtual Node::NodeType getNodeTypeForNodeWithId(Id id) const;

	virtual std::shared_ptr<TokenLocationCollection> getFullTextSearchLocations(
			const std::string& searchTerm, bool caseSensitive) const;
	virtual std::vector<SearchMatch> getAutocompletionMatches(const std::string& query) const;
	virtual std::vector<SearchMatch> getSearchMatchesForTokenIds(const std::vector<Id>& tokenIds) const;

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
	virtual std::vector<ErrorInfo> getAllErrors() const;

	virtual std::shared_ptr<TokenLocationCollection> getErrorTokenLocations(std::vector<ErrorInfo>* errors) const;

private:
	void handleMessage(MessageErrorFilterChanged* message);

	StorageAccess* m_subject;

	ErrorFilter m_errorFilter;
};

#endif // STORAGE_ACCESS_PROXY_H
