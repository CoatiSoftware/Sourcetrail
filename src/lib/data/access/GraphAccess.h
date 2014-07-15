#ifndef GRAPH_ACCESS_H
#define GRAPH_ACCESS_H

#include <string>
#include <vector>

#include "utility/types.h"

class Edge;
class Token;

class GraphAccess
{
public:
	virtual ~GraphAccess();

	virtual Id getIdForNodeWithName(const std::string& name) const = 0;
	virtual std::string getNameForNodeWithId(Id id) const = 0;
	virtual std::vector<std::string> getNamesForNodesWithNamePrefix(const std::string& prefix) const = 0;
	virtual std::vector<Id> getIdsOfNeighbours(const Id id) const = 0;
	virtual std::vector<std::pair<Id, Id>> getNeighbourEdgesOfNode(const Id id) const = 0;
	virtual std::vector<std::pair<Id, Id>> getMemberEdgesOfNode(const Id id) const = 0;
	virtual std::vector<std::pair<Id, Id>> getUsageEdgesOfNode(const Id id) const = 0;
	virtual std::vector<std::pair<Id, Id>> getCallEdgesOfNode(const Id id) const = 0;
	virtual std::vector<std::pair<Id, Id>> getTypeOfEdgesOfNode(const Id id) const = 0;
	virtual std::vector<std::pair<Id, Id>> getReturnTypeOfEdgesOfNode(const Id id) const = 0;
	virtual std::vector<std::pair<Id, Id>> getParameterOfEdgesOfNode(const Id id) const = 0;
};


#endif // GRAPH_ACCESS_H
