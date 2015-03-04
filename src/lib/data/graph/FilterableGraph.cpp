#include "data/graph/FilterableGraph.h"

#include "data/graph/Edge.h"
#include "data/graph/Node.h"

FilterableGraph::FilterableGraph()
{
}

FilterableGraph::~FilterableGraph()
{
}

size_t FilterableGraph::size() const
{
	return getNodeCount() + getEdgeCount();
}

Token* FilterableGraph::getTokenById(Id id) const
{
	Token* token = getNodeById(id);
	if (!token)
	{
		token = getEdgeById(id);
	}
	return token;
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

void FilterableGraph::printBasic(std::ostream& ostream) const
{
	ostream << getNodeCount() << " nodes:";
	forEachNode(
		[&ostream](Node* n)
		{
			if (n->isType(Node::NODE_FILE))
			{
				ostream << ' ' << n->getTypeString() << ':' << n->getName();
			}
			else
			{
				ostream << ' ' << n->getTypeString() << ':' << n->getFullName();
			}
		}
	);
	ostream << '\n';

	ostream << getEdgeCount() << " edges:";
	forEachEdge(
		[&ostream](Edge* e)
		{
			ostream << ' ' << e->getName();
		}
	);
	ostream << '\n';
}
