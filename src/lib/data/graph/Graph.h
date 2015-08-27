#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <memory>
#include <deque>

#include "data/graph/Edge.h"
#include "data/graph/Node.h"

class Graph
{
public:
	Graph();
	virtual ~Graph();

	void clear();

	void forEachNode(std::function<void(Node*)> func) const;
	void forEachEdge(std::function<void(Edge*)> func) const;
	void forEachToken(std::function<void(Token*)> func) const;

	Node* createNode(Id id, Node::NodeType type, std::shared_ptr<TokenComponentName> nameComponent);
	Edge* createEdge(Id id, Edge::EdgeType type, Node* from, Node* to);

	size_t getNodeCount() const;
	size_t getEdgeCount() const;

	Node* getNodeById(Id id) const;
	Edge* getEdgeById(Id id) const;

	const std::map<Id, std::shared_ptr<Node>>& getNodes() const;
	const std::map<Id, std::shared_ptr<Edge>>& getEdges() const;

	void removeNode(Node* node);
	void removeEdge(Edge* edge);
	bool removeNodeIfUnreferencedRecursive(Node* node);

	Node* findNode(std::function<bool(Node*)> func) const;
	Edge* findEdge(std::function<bool(Edge*)> func) const;
	Token* findToken(std::function<bool(Token*)> func) const;

	Node* addNodeAsPlainCopy(Node* node);
	Edge* addEdgeAsPlainCopy(Edge* edge);

	Node* addNodeAndAllChildrenAsPlainCopy(Node* node);
	Edge* addEdgeAndAllChildrenAsPlainCopy(Edge* edge);

	size_t size() const;

	Token* getTokenById(Id id) const;

	void print(std::ostream& ostream) const;
	void printBasic(std::ostream& ostream) const;

private:
	Graph(const Graph&);
	void operator=(const Graph&);

	void removeEdgeInternal(Edge* edge);

	std::map<Id, std::shared_ptr<Node>> m_nodes;
	std::map<Id, std::shared_ptr<Edge>> m_edges;
};

std::ostream& operator<<(std::ostream& ostream, const Graph& graph);

#endif // GRAPH_H
