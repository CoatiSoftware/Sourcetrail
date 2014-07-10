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
	virtual std::vector<std::string> getNamesForNodesWithNamePrefix(const std::string& prefix) const;
	virtual std::vector<Id> getIdsOfNeighbours(const Id id) const;
	virtual std::vector<std::pair<Id, Id>> getConnectedEdges(const Id id) const;

private:
	GraphAccess* m_subject;
};

#endif // GRAPH_ACCESS_PROXY_H
