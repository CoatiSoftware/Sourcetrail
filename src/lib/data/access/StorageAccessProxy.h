#ifndef STORAGE_ACCESS_PROXY_H
#define STORAGE_ACCESS_PROXY_H

#include "data/access/StorageAccess.h"

class StorageAccessProxy: public StorageAccess
{
public:
	StorageAccessProxy();
	virtual ~StorageAccessProxy();

	bool hasSubject() const;
	void setSubject(StorageAccess* subject);

	// StorageAccess implementation
	virtual Id getIdForNodeWithNameHierarchy(const NameHierarchy& nameHierarchy) const;
	virtual Id getIdForNodeWithSearchNameHierarchy(const NameHierarchy& nameHierarchy) const;
	virtual Id getIdForEdge(
		Edge::EdgeType type, const NameHierarchy& fromNameHierarchy, const NameHierarchy& toNameHierarchy) const;

	virtual Id getIdForFirstNode() const;

	virtual NameHierarchy getNameHierarchyForNodeWithId(Id id) const;
	virtual Node::NodeType getNodeTypeForNodeWithId(Id id) const;

	virtual std::vector<SearchMatch> getAutocompletionMatches(const std::string& query) const;
	virtual std::vector<SearchMatch> getSearchMatchesForTokenIds(const std::vector<Id>& tokenIds) const;

	virtual std::shared_ptr<Graph> getGraphForAll() const;
	virtual std::shared_ptr<Graph> getGraphForActiveTokenIds(const std::vector<Id>& tokenIds, bool activeOnly) const;

	virtual std::vector<Id> getActiveTokenIdsForTokenIds(const std::vector<Id>& tokenIds) const;
	virtual std::vector<Id> getActiveTokenIdsForId(Id tokenId, Id* declarationId) const;

	virtual std::vector<Id> getNodeIdsForLocationIds(const std::vector<Id>& locationIds) const;

	virtual std::vector<Id> getTokenIdsForMatches(const std::vector<SearchMatch>& matches) const;
	virtual Id getTokenIdForFileNode(const FilePath& filePath) const;
	virtual std::vector<Id> getTokenIdsForAggregationEdge(Id sourceId, Id targetId) const;

	virtual std::shared_ptr<TokenLocationCollection> getTokenLocationsForTokenIds(const std::vector<Id>& tokenIds) const;
	virtual std::shared_ptr<TokenLocationCollection> getTokenLocationsForLocationIds(const std::vector<Id>& locationIds) const;
	virtual std::shared_ptr<TokenLocationFile> getTokenLocationsForFile(const std::string& filePath) const;
	virtual std::shared_ptr<TokenLocationFile> getTokenLocationsForLinesInFile(
		const std::string& filePath, uint firstLineNumber, uint lastLineNumber
	) const;

	virtual TokenLocationCollection getErrorTokenLocations(std::vector<std::string>* errorMessages) const;
	virtual std::shared_ptr<TokenLocationFile> getCommentLocationsInFile(const FilePath& filePath) const;

	virtual std::shared_ptr<TextAccess> getFileContent(const FilePath& filePath) const;
	virtual TimePoint getFileModificationTime(const FilePath& filePath) const;

	virtual StorageStats getStorageStats() const;

private:
	StorageAccess* m_subject;
};

#endif // STORAGE_ACCESS_PROXY_H
