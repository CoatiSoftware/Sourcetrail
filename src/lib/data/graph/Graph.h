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
	enum TrailMode
	{
		TRAIL_NONE,
		TRAIL_HORIZONTAL,
		TRAIL_VERTICAL
	};

	Graph();
	virtual ~Graph();

	void clear();

	void forEachNode(std::function<void(Node*)> func) const;
	void forEachEdge(std::function<void(Edge*)> func) const;
	void forEachToken(std::function<void(Token*)> func) const;

	Node* createNode(Id id, NodeType type, const NameHierarchy& nameHierarchy, bool defined);
	Edge* createEdge(Id id, Edge::EdgeType type, Node* from, Node* to);

	size_t getNodeCount() const;
	size_t getEdgeCount() const;

	Node* getNodeById(Id id) const;
	Edge* getEdgeById(Id id) const;

	const std::map<Id, std::shared_ptr<Node>>& getNodes() const;
	const std::map<Id, std::shared_ptr<Edge>>& getEdges() const;

	void removeNode(Node* node);
	void removeEdge(Edge* edge);

	Node* findNode(std::function<bool(Node*)> func) const;
	Edge* findEdge(std::function<bool(Edge*)> func) const;
	Token* findToken(std::function<bool(Token*)> func) const;

	Node* addNodeAsPlainCopy(Node* node);
	Edge* addEdgeAsPlainCopy(Edge* edge);

	Node* addNodeAndAllChildrenAsPlainCopy(Node* node);
	Edge* addEdgeAndAllChildrenAsPlainCopy(Edge* edge);

	size_t size() const;

	Token* getTokenById(Id id) const;

	TrailMode getTrailMode() const;
	void setTrailMode(TrailMode trailMode);

	bool hasTrailOrigin() const;
	void setHasTrailOrigin(bool hasOrigin);

	void print(std::ostream& ostream) const;
	void printBasic(std::ostream& ostream) const;

private:
	Graph(const Graph&);
	void operator=(const Graph&);

	void removeEdgeInternal(Edge* edge);

	std::map<Id, std::shared_ptr<Node>> m_nodes;
	std::map<Id, std::shared_ptr<Edge>> m_edges;

	TrailMode m_trailMode;
	bool m_hasTrailOrigin;
};

std::ostream& operator<<(std::ostream& ostream, const Graph& graph);

#endif // GRAPH_H
