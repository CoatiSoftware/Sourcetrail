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
	Graph& operator=(const Graph& other);

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

	Node* getNode(const std::string& fullName) const;
	Edge* getEdge(Edge::EdgeType type, Node* from, Node* to) const;

	Node* getNodeById(Id id) const;
	Edge* getEdgeById(Id id) const;
	Token* getTokenById(Id id) const;

	Node* createNodeHierarchy(const std::string& fullName);
	Node* createNodeHierarchy(Node::NodeType type, const std::string& fullName);

	Node* createNodeHierarchyWithDistinctSignature(const std::string& fullName, const std::string& signature);
	Node* createNodeHierarchyWithDistinctSignature(
		Node::NodeType type, const std::string& fullName, const std::string& signature
	);

	Edge* createEdge(Edge::EdgeType type, Node* from, Node* to);

	void removeNode(Node* node);
	void removeEdge(Edge* edge);

	Node* findNode(std::function<bool(Node*)> func) const;
	Edge* findEdge(std::function<bool(Edge*)> func) const;
	Token* findToken(std::function<bool(Token*)> func) const;

	Node* addNodeAsPlainCopy(Node* node);
	Edge* addEdgeAsPlainCopy(Edge* edge);

private:
	static const std::string DELIMITER;

	Node* getLastValidNode(std::deque<std::string>* names) const;
	Node* insertNodeHierarchy(Node::NodeType type, std::deque<std::string> names, Node* parentNode);
	Node* insertNode(Node::NodeType type, const std::string& name, Node* parentNode);
	Edge* insertEdge(Edge::EdgeType type, Node* from, Node* to);
	void removeEdgeInternal(Edge* edge);

	std::map<Id, std::shared_ptr<Node>> m_nodes;
	std::map<Id, std::shared_ptr<Edge>> m_edges;
};

std::ostream& operator<<(std::ostream& ostream, const Graph& graph);

#endif // GRAPH_H
