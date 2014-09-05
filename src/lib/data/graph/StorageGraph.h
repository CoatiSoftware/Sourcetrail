#ifndef STORAGE_GRAPH_H
#define STORAGE_GRAPH_H

#include "data/graph/Graph.h"
#include "data/SearchIndex.h"

class StorageGraph
	: public Graph
{
public:
	StorageGraph();
	virtual ~StorageGraph();

	Node* createNodeHierarchy(Node::NodeType type, SearchIndex::SearchNode* searchNode);
	Node* createNodeHierarchyWithDistinctSignature(
		Node::NodeType type, SearchIndex::SearchNode* searchNode, const std::string& signature);
	Edge* createEdge(Edge::EdgeType type, Node* from, Node* to);

private:
	Node* insertNodeHierarchy(Node::NodeType type, SearchIndex::SearchNode* searchNode);
	Node* insertNode(Node::NodeType type, Node* parentNode, SearchIndex::SearchNode* searchNode);
	Edge* insertEdge(Edge::EdgeType type, Node* from, Node* to);
};

#endif // STORAGE_GRAPH_H
