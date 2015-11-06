#ifndef STORAGE_ACCESS_H
#define STORAGE_ACCESS_H

#include <memory>
#include <string>
#include <vector>

#include "utility/types.h"
#include "utility/file/FilePath.h"

#include "data/graph/Node.h"
#include "data/search/SearchMatch.h"

struct FileInfo;
class Graph;
class TextAccess;
class TokenLocation;
class TokenLocationCollection;
class TokenLocationFile;
class TimePoint;

class StorageAccess
{
public:
	virtual ~StorageAccess();

	virtual Id getIdForNodeWithNameHierarchy(const NameHierarchy& nameHierarchy) const = 0;
	virtual Id getIdForEdge(
		Edge::EdgeType type, const NameHierarchy& fromNameHierarchy, const NameHierarchy& toNameHierarchy) const = 0;

	virtual Id getIdForFirstNode() const = 0;

	virtual NameHierarchy getNameHierarchyForNodeWithId(Id id) const = 0;
	virtual Node::NodeType getNodeTypeForNodeWithId(Id id) const = 0;

	virtual std::vector<SearchMatch> getAutocompletionMatches(const std::string& query) const = 0;
	virtual std::vector<SearchMatch> getSearchMatchesForTokenIds(const std::vector<Id>& tokenIds) const = 0;

	virtual std::shared_ptr<Graph> getGraphForActiveTokenIds(const std::vector<Id>& tokenIds) const = 0;

	virtual std::vector<Id> getActiveTokenIdsForTokenIds(const std::vector<Id>& tokenIds) const = 0;
	virtual std::vector<Id> getActiveTokenIdsForId(Id tokenId, Id* declarationId) const = 0;

	virtual std::vector<Id> getNodeIdsForLocationIds(const std::vector<Id>& locationIds) const = 0;

	virtual std::vector<Id> getTokenIdsForMatches(const std::vector<SearchMatch>& matches) const = 0;
	virtual Id getTokenIdForFileNode(const FilePath& filePath) const = 0;
	virtual std::vector<Id> getTokenIdsForAggregationEdge(Id sourceId, Id targetId) const = 0;

	virtual std::shared_ptr<TokenLocationCollection> getTokenLocationsForTokenIds(const std::vector<Id>& tokenIds) const = 0;
	virtual std::shared_ptr<TokenLocationCollection> getTokenLocationsForLocationIds(const std::vector<Id>& locationIds) const = 0;
	virtual std::shared_ptr<TokenLocationFile> getTokenLocationsForFile(const std::string& filePath) const = 0;
	virtual std::shared_ptr<TokenLocationFile> getTokenLocationsForLinesInFile(
		const std::string& filePath, uint firstLineNumber, uint lastLineNumber) const = 0;

	virtual TokenLocationCollection getErrorTokenLocations(std::vector<std::string>* errorMessages) const = 0;
	virtual std::shared_ptr<TokenLocationFile> getTokenLocationOfParentScope(const TokenLocation* child) const = 0;
	virtual std::shared_ptr<TokenLocationFile> getCommentLocationsInFile(const FilePath& filePath) const = 0;

	virtual std::shared_ptr<TextAccess> getFileContent(const FilePath& filePath) const = 0;
	virtual TimePoint getFileModificationTime(const FilePath& filePath) const = 0;
};

#endif // STORAGE_ACCESS_H
