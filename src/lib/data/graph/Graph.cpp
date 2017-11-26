#include "data/graph/Graph.h"

#include "utility/logging/logging.h"

Graph::Graph()
	: m_trailMode(TRAIL_NONE)
{
}

Graph::~Graph()
{
	m_edges.clear();
	m_nodes.clear();
}

void Graph::clear()
{
	m_edges.clear();
	m_nodes.clear();
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

Node* Graph::createNode(Id id, NodeType type, NameHierarchy nameHierarchy, bool defined)
{
	Node* n = getNodeById(id);
	if (n)
	{
		return n;
	}

	std::shared_ptr<Node> node = std::make_shared<Node>(id, type, nameHierarchy, defined);
	m_nodes.emplace(node->getId(), node);
	return node.get();
}

Edge* Graph::createEdge(Id id, Edge::EdgeType type, Node* from, Node* to)
{
	Edge* e = getEdgeById(id);
	if (e)
	{
		return e;
	}

	if (!getNodeById(from->getId()) || !getNodeById(to->getId()))
	{
		LOG_ERROR("Can't add edge, without adding the nodes first.");
		return nullptr;
	}

	std::shared_ptr<Edge> edge = std::make_shared<Edge>(id, type, from, to);
	m_edges.emplace(edge->getId(), edge);
	return edge.get();
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

	if (node->getEdgeCount())
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

size_t Graph::size() const
{
	return getNodeCount() + getEdgeCount();
}

Token* Graph::getTokenById(Id id) const
{
	Token* token = getNodeById(id);
	if (!token)
	{
		token = getEdgeById(id);
	}
	return token;
}

Graph::TrailMode Graph::getTrailMode() const
{
	return m_trailMode;
}

void Graph::setTrailMode(TrailMode trailMode)
{
	m_trailMode = trailMode;
}

bool Graph::hasTrailOrigin() const
{
	return m_hasTrailOrigin;
}

void Graph::setHasTrailOrigin(bool hasOrigin)
{
	m_hasTrailOrigin = hasOrigin;
}

void Graph::print(std::ostream& ostream) const
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

void Graph::printBasic(std::ostream& ostream) const
{
	ostream << getNodeCount() << " nodes:";
	forEachNode(
		[&ostream](Node* n)
		{
			ostream << ' ' << n->getReadableTypeString() << ':' << n->getFullName();
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
