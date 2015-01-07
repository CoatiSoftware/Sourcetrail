#ifndef STORAGE_GRAPH_H
#define STORAGE_GRAPH_H

#include "data/graph/Graph.h"
#include "data/graph/token_component/TokenComponentSignature.h"
#include "data/search/SearchNode.h"

class StorageGraph
	: public Graph
{
public:
	StorageGraph();
	virtual ~StorageGraph();

	Node* createNodeHierarchy(Node::NodeType type, SearchNode* searchNode);
	Node* createNodeHierarchyWithDistinctSignature(
		Node::NodeType type, SearchNode* searchNode, std::shared_ptr<TokenComponentSignature> signature);
	Edge* createEdge(Edge::EdgeType type, Node* from, Node* to);

private:
	Node* insertNodeHierarchy(Node::NodeType type, SearchNode* searchNode);
	Node* insertNode(Node::NodeType type, Node* parentNode, SearchNode* searchNode);
	Edge* insertEdge(Edge::EdgeType type, Node* from, Node* to);

	void updateAggregationEdges(Node* from, Node* to, Id edgeId);
};

#endif // STORAGE_GRAPH_H
