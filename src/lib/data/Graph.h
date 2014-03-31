#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

#include "data/Edge.h"
#include "data/Node.h"

class Graph
{
public:
	Graph();
	~Graph();

private:
	std::vector<std::shared_ptr<Node> > m_nodes;
	std::vector<std::shared_ptr<Edge> > m_edges;
};


#endif // GRAPH_H
