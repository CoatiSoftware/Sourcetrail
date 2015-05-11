#ifndef STORAGE_ACCESS_H
#define STORAGE_ACCESS_H

#include <memory>
#include <string>
#include <vector>

#include "utility/types.h"

#include "data/graph/Node.h"
#include "data/search/SearchMatch.h"

class Graph;
class TokenLocation;
class TokenLocationCollection;
class TokenLocationFile;

class StorageAccess
{
public:
	virtual ~StorageAccess();

	virtual Id getIdForNodeWithName(const std::string& name) const = 0;
	virtual std::string getNameForNodeWithId(Id id) const = 0;
	virtual Node::NodeType getNodeTypeForNodeWithId(Id id) const = 0;
	virtual std::vector<SearchMatch> getAutocompletionMatches(
		const std::string& query, const std::string& word) const = 0;

	virtual std::shared_ptr<Graph> getGraphForActiveTokenIds(const std::vector<Id>& tokenIds) const = 0;

	virtual std::vector<Id> getActiveTokenIdsForId(Id tokenId, Id* declarationId) const = 0;
	virtual std::vector<Id> getActiveTokenIdsForLocationId(Id locationId) const = 0;

	virtual std::vector<Id> getTokenIdsForQuery(std::string query) const = 0;

	virtual TokenLocationCollection getTokenLocationsForTokenIds(const std::vector<Id>& tokenIds) const = 0;
	virtual std::shared_ptr<TokenLocationFile> getTokenLocationsForFile(const std::string& filePath) const = 0;
	virtual std::shared_ptr<TokenLocationFile> getTokenLocationsForLinesInFile(
		const std::string& filePath, uint firstLineNumber, uint lastLineNumber) const = 0;

	virtual TokenLocationCollection getErrorTokenLocations(std::vector<std::string>* errorMessages) const = 0;
	virtual std::shared_ptr<TokenLocationFile> getTokenLocationOfParentScope(const TokenLocation* child) const = 0;
};

#endif // STORAGE_ACCESS_H
