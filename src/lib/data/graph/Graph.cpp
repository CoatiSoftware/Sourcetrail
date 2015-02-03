#include "data/graph/Graph.h"

#include "utility/logging/logging.h"

Graph::Graph()
{
}

Graph::~Graph()
{
	m_edges.clear();
	m_nodes.clear();
}

void Graph::copy(const FilterableGraph* other)
{
	clear();
	add(other);
}

void Graph::clear()
{
	m_edges.clear();
	m_nodes.clear();
}

void Graph::add(const FilterableGraph* other)
{
	other->forEachNode(std::bind(&Graph::addNode, this, std::placeholders::_1));
	other->forEachEdge(std::bind(&Graph::addEdge, this, std::placeholders::_1));
}

void Graph::forEachNode(std::function<void(Node*)> func) const
{
	for (const std::pair<Id, std::shared_ptr<Node>>& node : m_nodes)
	{
		func(node.second.get());
	}
}

void Graph::forEachEdge(std::function<void(Edge*)> func) const
{
	for (const std::pair<Id, std::shared_ptr<Edge>>& edge : m_edges)
	{
		func(edge.second.get());
	}
}

void Graph::forEachToken(std::function<void(Token*)> func) const
{
	forEachNode(func);
	forEachEdge(func);
}

void Graph::addNode(Node* node)
{
	addNodeAsPlainCopy(node);
}

void Graph::addEdge(Edge* edge)
{
	if (getNodeById(edge->getFrom()->getId()) && getNodeById(edge->getTo()->getId()))
	{
		addEdgeAsPlainCopy(edge);
	}
}

size_t Graph::getNodeCount() const
{
	return m_nodes.size();
}

size_t Graph::getEdgeCount() const
{
	return m_edges.size();
}

Node* Graph::getNodeById(Id id) const
{
	std::map<Id, std::shared_ptr<Node>>::const_iterator it = m_nodes.find(id);
	if (it != m_nodes.end())
	{
		return it->second.get();
	}
	return nullptr;
}

Edge* Graph::getEdgeById(Id id) const
{
	std::map<Id, std::shared_ptr<Edge>>::const_iterator it = m_edges.find(id);
	if (it != m_edges.end())
	{
		return it->second.get();
	}
	return nullptr;
}

const std::map<Id, std::shared_ptr<Node>>& Graph::getNodes() const
{
	return m_nodes;
}

const std::map<Id, std::shared_ptr<Edge>>& Graph::getEdges() const
{
	return m_edges;
}

void Graph::removeNode(Node* node)
{
	std::map<Id, std::shared_ptr<Node>>::const_iterator it = m_nodes.find(node->getId());
	if (it == m_nodes.end())
	{
		LOG_WARNING("Node was not found in the graph.");
		return;
	}

	node->forEachEdgeOfType(
		Edge::EDGE_MEMBER,
		[this, node](Edge* e)
		{
			if (node == e->getFrom())
			{
				this->removeNode(e->getTo());
			}
		}
	);

	node->forEachEdge(
		[this](Edge* e)
		{
			this->removeEdgeInternal(e);
		}
	);

	if (node->getEdges().size())
	{
		LOG_ERROR("Node still has edges.");
	}

	m_nodes.erase(it);
}

void Graph::removeEdge(Edge* edge)
{
	std::map<Id, std::shared_ptr<Edge>>::const_iterator it = m_edges.find(edge->getId());
	if (it == m_edges.end())
	{
		LOG_WARNING("Edge was not found in the graph.");
	}

	if (edge->getType() == Edge::EDGE_MEMBER)
	{
		LOG_ERROR("Can't remove member edge, without removing the child node.");
		return;
	}

	m_edges.erase(it);
}

bool Graph::removeNodeIfUnreferencedRecursive(Node* node)
{
	if (!node->hasReferences())
	{
		Node* parent = node->getParentNode();

		removeNode(node);

		if (parent)
		{
			removeNodeIfUnreferencedRecursive(parent);
		}

		return true;
	}

	return false;
}

Node* Graph::findNode(std::function<bool(Node*)> func) const
{
	std::map<Id, std::shared_ptr<Node>>::const_iterator it = find_if(m_nodes.begin(), m_nodes.end(),
		[&func](const std::pair<Id, std::shared_ptr<Node>>& n)
		{
			return func(n.second.get());
		}
	);

	if (it != m_nodes.end())
	{
		return it->second.get();
	}

	return nullptr;
}

Edge* Graph::findEdge(std::function<bool(Edge*)> func) const
{
	std::map<Id, std::shared_ptr<Edge>>::const_iterator it = find_if(m_edges.begin(), m_edges.end(),
		[func](const std::pair<Id, std::shared_ptr<Edge>>& e)
		{
			return func(e.second.get());
		}
	);

	if (it != m_edges.end())
	{
		return it->second.get();
	}

	return nullptr;
}

Token* Graph::findToken(std::function<bool(Token*)> func) const
{
	Node* node = findNode(func);
	if (node)
	{
		return node;
	}

	Edge* edge = findEdge(func);
	if (edge)
	{
		return edge;
	}

	return nullptr;
}

Node* Graph::addNodeAsPlainCopy(Node* node)
{
	Node* n = getNodeById(node->getId());
	if (n)
	{
		return n;
	}

	std::shared_ptr<Node> copy = std::make_shared<Node>(*node);
	m_nodes.emplace(copy->getId(), copy);
	return copy.get();
}

Edge* Graph::addEdgeAsPlainCopy(Edge* edge)
{
	Edge* e = getEdgeById(edge->getId());
	if (e)
	{
		return e;
	}

	Node* from = addNodeAsPlainCopy(edge->getFrom());
	Node* to = addNodeAsPlainCopy(edge->getTo());

	std::shared_ptr<Edge> copy = std::make_shared<Edge>(*edge, from, to);
	m_edges.emplace(copy->getId(), copy);
	return copy.get();
}

Node* Graph::addNodeAndAllChildrenAsPlainCopy(Node* node)
{
	Node* n = addNodeAsPlainCopy(node);

	node->forEachEdgeOfType(Edge::EDGE_MEMBER,
		[node, this](Edge* edge)
		{
			if (edge->getFrom() == node)
			{
				addEdgeAsPlainCopy(edge);
				addNodeAndAllChildrenAsPlainCopy(edge->getTo());
			}
		}
	);

	return n;
}

Edge* Graph::addEdgeAndAllChildrenAsPlainCopy(Edge* edge)
{
	addNodeAndAllChildrenAsPlainCopy(edge->getFrom()->getLastParentNode());
	addNodeAndAllChildrenAsPlainCopy(edge->getTo()->getLastParentNode());

	return addEdgeAsPlainCopy(edge);
}

void Graph::removeEdgeInternal(Edge* edge)
{
	std::map<Id, std::shared_ptr<Edge> >::const_iterator it = m_edges.find(edge->getId());
	if (it != m_edges.end() && it->second.get() == edge)
	{
		m_edges.erase(it);
		return;
	}
}

std::ostream& operator<<(std::ostream& ostream, const Graph& graph)
{
	graph.print(ostream);
	return ostream;
}
