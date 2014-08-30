#include "data/graph/FilterableGraph.h"

#include "data/graph/Edge.h"
#include "data/graph/Node.h"

FilterableGraph::FilterableGraph()
{
}

FilterableGraph::~FilterableGraph()
{
}

void FilterableGraph::print(std::ostream& ostream) const
{
	ostream << "Graph:\n";
	ostream << "nodes (" << getNodeCount() << ")\n";
	forEachNode(
		[&ostream](Node* n)
		{
			ostream << *n << '\n';
		}
	);

	ostream << "edges (" << getEdgeCount() << ")\n";
	forEachEdge(
		[&ostream](Edge* e)
		{
			ostream << *e << '\n';
		}
	);
}
