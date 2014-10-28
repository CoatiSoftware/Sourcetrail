#include "data/graph/SubGraph.h"

#include "data/graph/Edge.h"
#include "data/graph/Node.h"

SubGraph::SubGraph()
{
}

SubGraph::~SubGraph()
{
}

void SubGraph::copy(const FilterableGraph* other)
{
	clear();
	add(other);
}

void SubGraph::clear()
{
	m_edges.clear();
	m_nodes.clear();
}

void SubGraph::add(const FilterableGraph* other)
{
	other->forEachNode(std::bind(&SubGraph::addNode, this, std::placeholders::_1));
	other->forEachEdge(std::bind(&SubGraph::addEdge, this, std::placeholders::_1));
}

void SubGraph::forEachNode(std::function<void(Node*)> func) const
{
	for (const std::pair<Id, Node*>& node : m_nodes)
	{
		func(node.second);
	}
}

void SubGraph::forEachEdge(std::function<void(Edge*)> func) const
{
	for (const std::pair<Id, Edge*>& edge : m_edges)
	{
		func(edge.second);
	}
}

void SubGraph::forEachToken(std::function<void(Token*)> func) const
{
	forEachNode(func);
	forEachEdge(func);
}

void SubGraph::addNode(Node* node)
{
	m_nodes.emplace(node->getId(), node);
}

void SubGraph::addEdge(Edge* edge)
{
	m_edges.emplace(edge->getId(), edge);
}

size_t SubGraph::getNodeCount() const
{
	return m_nodes.size();
}

size_t SubGraph::getEdgeCount() const
{
	return m_edges.size();
}

Node* SubGraph::getNodeById(Id id) const
{
	std::map<Id, Node*>::const_iterator it = m_nodes.find(id);
	if (it != m_nodes.end())
	{
		return it->second;
	}
	return nullptr;
}

Edge* SubGraph::getEdgeById(Id id) const
{
	std::map<Id, Edge*>::const_iterator it = m_edges.find(id);
	if (it != m_edges.end())
	{
		return it->second;
	}
	return nullptr;
}

const std::map<Id, Node*>& SubGraph::getNodes() const
{
	return m_nodes;
}

const std::map<Id, Edge*>& SubGraph::getEdges() const
{
	return m_edges;
}

std::vector<Id> SubGraph::getTokenIds() const
{
	std::vector<Id> ids;
	for (const std::pair<Id, Node*>& node : m_nodes)
	{
		ids.push_back(node.first);
	}

	for (const std::pair<Id, Edge*>& edge : m_edges)
	{
		ids.push_back(edge.first);
	}
	return ids;
}

void SubGraph::subtract(const SubGraph& other)
{
	for (const std::pair<Id, Node*>& node : other.m_nodes)
	{
		m_nodes.erase(node.first);
	}

	for (const std::pair<Id, Edge*>& edge : other.m_edges)
	{
		m_edges.erase(edge.first);
	}
}

std::ostream& operator<<(std::ostream& ostream, const SubGraph& graph)
{
	graph.print(ostream);
	return ostream;
}
