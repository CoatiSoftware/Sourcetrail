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
	virtual std::vector<std::tuple<Id, Id, Id>> getNeighbourEdgesOfNode(const Id id) const;
	virtual std::vector<std::tuple<Id, Id, Id>> getMemberEdgesOfNode(const Id id) const;
	virtual std::vector<std::tuple<Id, Id, Id>> getUsageEdgesOfNode(const Id id) const;
	virtual std::vector<std::tuple<Id, Id, Id>> getCallEdgesOfNode(const Id id) const;
	virtual std::vector<std::tuple<Id, Id, Id>> getTypeOfEdgesOfNode(const Id id) const;
	virtual std::vector<std::tuple<Id, Id, Id>> getReturnTypeOfEdgesOfNode(const Id id) const;
	virtual std::vector<std::tuple<Id, Id, Id>> getParameterOfEdgesOfNode(const Id id) const;
	virtual std::vector<std::tuple<Id, Id, Id>> getInheritanceEdgesOfNode(const Id id) const;

	virtual std::pair<Id, Id> getNodesOfEdge(const Id id) const;

	virtual bool checkTokenIsNode(const Id id) const;

private:
	GraphAccess* m_subject;
};

#endif // GRAPH_ACCESS_PROXY_H
