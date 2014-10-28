#ifndef GRAPH_ACCESS_H
#define GRAPH_ACCESS_H

#include <memory>
#include <string>
#include <vector>

#include "data/graph/Graph.h"
#include "data/SearchIndex.h"
#include "utility/types.h"

class GraphAccess
{
public:
	virtual ~GraphAccess();

	virtual Id getIdForNodeWithName(const std::string& name) const = 0;
	virtual std::string getNameForNodeWithId(Id id) const = 0;
	virtual std::vector<SearchIndex::SearchMatch> getAutocompletionMatches(
		const std::string& query, const std::string& word) const = 0;

	virtual std::shared_ptr<Graph> getGraphForActiveTokenIds(const std::vector<Id>& tokenIds) const = 0;

	virtual std::vector<Id> getActiveTokenIdsForId(Id tokenId, Id& declarationId) const = 0;
	virtual std::vector<Id> getLocationIdsForTokenIds(const std::vector<Id>& tokenIds) const = 0;

	virtual std::vector<Id> getTokenIdsForQuery(std::string query) const = 0;
};

#endif // GRAPH_ACCESS_H
