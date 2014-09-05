#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <memory>
#include <deque>

#include "data/graph/Edge.h"
#include "data/graph/FilterableGraph.h"
#include "data/graph/Node.h"

class Graph
	: public FilterableGraph
{
public:
	Graph();
	virtual ~Graph();

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

	const std::map<Id, std::shared_ptr<Node>>& getNodes() const;
	const std::map<Id, std::shared_ptr<Edge>>& getEdges() const;

	Node* getNodeById(Id id) const;
	Edge* getEdgeById(Id id) const;
	Token* getTokenById(Id id) const;

	void removeNode(Node* node);
	void removeEdge(Edge* edge);

	Node* findNode(std::function<bool(Node*)> func) const;
	Edge* findEdge(std::function<bool(Edge*)> func) const;
	Token* findToken(std::function<bool(Token*)> func) const;

	Node* addNodeAsPlainCopy(Node* node);
	Edge* addEdgeAsPlainCopy(Edge* edge);

protected:
	std::map<Id, std::shared_ptr<Node>> m_nodes;
	std::map<Id, std::shared_ptr<Edge>> m_edges;

private:
	Graph(const Graph&);
	void operator=(const Graph&);

	void removeEdgeInternal(Edge* edge);
};

std::ostream& operator<<(std::ostream& ostream, const Graph& graph);

#endif // GRAPH_H
