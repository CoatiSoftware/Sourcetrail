#ifndef GRAPH_H
#define GRAPH_H

#include <memory>
#include <vector>
#include <ostream>

#include "data/graph/Edge.h"
#include "data/graph/Node.h"

class Graph
{
public:
	Graph();
	virtual ~Graph();

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

	void forEachNode(std::function<void(Node*)> func) const;
	void forEachEdge(std::function<void(Edge*)> func) const;
	void forEachToken(std::function<void(Token*)> func) const;

	Node* addNodeAsPlainCopy(Node* node);
	Edge* addEdgeAsPlainCopy(Edge* edge);

	void clear();

protected:
	const std::vector<std::shared_ptr<Node> >& getNodes() const;
	const std::vector<std::shared_ptr<Edge> >& getEdges() const;

private:
	static const std::string DELIMITER;

	Node* insertNodeHierarchy(Node::NodeType type, const std::string& fullName);
	Node* insertNode(Node::NodeType type, const std::string& fullName, Node* parentNode);
	Edge* insertEdge(Edge::EdgeType type, Node* from, Node* to);
	void removeEdgeInternal(Edge* edge);

	const std::string m_delimiter;

	std::vector<std::shared_ptr<Node> > m_nodes;
	std::vector<std::shared_ptr<Edge> > m_edges;

	friend std::ostream& operator<<(std::ostream& ostream, const Graph& graph);
};

std::ostream& operator<<(std::ostream& ostream, const Graph& graph);

#endif // GRAPH_H
