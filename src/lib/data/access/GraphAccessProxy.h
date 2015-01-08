#ifndef GRAPH_ACCESS_PROXY_H
#define GRAPH_ACCESS_PROXY_H

#include "data/access/GraphAccess.h"

class GraphAccessProxy: public GraphAccess
{
public:
	GraphAccessProxy();
	virtual ~GraphAccessProxy();

	bool hasSubject() const;
	void setSubject(GraphAccess* subject);

	// GraphAccess implementation
	virtual Id getIdForNodeWithName(const std::string& name) const;
	virtual std::string getNameForNodeWithId(Id id) const;
	virtual std::vector<SearchMatch> getAutocompletionMatches(
		const std::string& query, const std::string& word) const;

	virtual std::shared_ptr<Graph> getGraphForActiveTokenIds(const std::vector<Id>& tokenIds) const;

	virtual std::vector<Id> getActiveTokenIdsForId(Id tokenId, Id* declarationId) const;
	virtual std::vector<Id> getActiveTokenIdsForLocationId(Id locationId) const;
	virtual std::vector<Id> getLocationIdsForTokenIds(const std::vector<Id>& tokenIds) const;

	virtual std::vector<Id> getTokenIdsForQuery(std::string query) const;

private:
	GraphAccess* m_subject;
};

#endif // GRAPH_ACCESS_PROXY_H
