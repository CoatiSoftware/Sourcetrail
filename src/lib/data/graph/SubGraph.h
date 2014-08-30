#ifndef SUB_GRAPH_H
#define SUB_GRAPH_H

#include <functional>
#include <map>
#include <vector>

#include "data/graph/FilterableGraph.h"
#include "utility/types.h"

class Edge;
class Node;
class Token;

class SubGraph
	: public FilterableGraph
{
public:
	SubGraph();
	virtual ~SubGraph();

	// FilterableGraph implementation
	virtual void copy(const FilterableGraph* other);
	virtual void clear();

	virtual void add(const FilterableGraph* other);

	virtual void forEachNode(std::function<void(Node*)> func) const;
	virtual void forEachEdge(std::function<void(Edge*)> func) const;
	virtual void forEachToken(std::function<void(Token*)> func) const;

	virtual void addNode(Node* node);
	virtual void addEdge(Edge* edge);

	virtual size_t getNodeCount() const;
	virtual size_t getEdgeCount() const;

	std::vector<Id> getTokenIds() const;

	void subtract(const SubGraph& other);

private:
	std::map<Id, Node*> m_nodes;
	std::map<Id, Edge*> m_edges;
};

std::ostream& operator<<(std::ostream& ostream, const SubGraph& graph);

#endif // SUB_GRAPH_H
