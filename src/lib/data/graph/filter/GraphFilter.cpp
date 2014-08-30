#include "data/graph/filter/GraphFilter.h"

#include "data/graph/Edge.h"
#include "data/graph/FilterableGraph.h"
#include "utility/logging/logging.h"

GraphFilter::GraphFilter()
	: m_outGraph(nullptr)
{
}

GraphFilter::~GraphFilter()
{
}

void GraphFilter::apply(const FilterableGraph* in, FilterableGraph* out)
{
	if (!in || !out)
	{
		LOG_ERROR("GraphFilter not called with correct pointers.");
		return;
	}
	else if (in == out)
	{
		LOG_ERROR("In and out graphs are the same.");
		return;
	}

	m_outGraph = out;

	in->forEachNode(
		[this](Node* node)
		{
			visitNode(node);
		}
	);

	// in->forEachEdge(
	// 	[this](Edge* edge)
	// 	{
	// 		visitEdge(edge);
	// 	}
	// );

	m_outGraph = nullptr;
}

void GraphFilter::visitNode(Node* node)
{
}

// void GraphFilter::visitEdge(Edge* edge)
// {
// }

void GraphFilter::addNode(Node* node)
{
	m_outGraph->addNode(node);
}

void GraphFilter::addEdge(Edge* edge)
{
	m_outGraph->addNode(edge->getFrom());
	m_outGraph->addNode(edge->getTo());
	m_outGraph->addEdge(edge);
}
