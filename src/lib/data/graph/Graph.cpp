#include "data/graph/Graph.h"

#include "utility/logging/logging.h"
#include "utility/utilityString.h"

Graph::Graph()
{
}

Graph::~Graph()
{
	m_edges.clear();
	m_nodes.clear();
}

Node* Graph::getNode(const std::string& fullName) const
{
	return findNode(
		[fullName](Node* n)
		{
			return n->getName() == fullName;
		}
	);
}

Edge* Graph::getEdge(Edge::EdgeType type, Node* from, Node* to) const
{
	return from->findEdgeOfType(type,
		[to](Edge* e)
		{
			return e->getTo() == to;
		}
	);
}

Node* Graph::getNodeById(Id id) const
{
	return findNode(
		[id](Node* n)
		{
			return n->getId() == id;
		}
	);
}

Edge* Graph::getEdgeById(Id id) const
{
	return findEdge(
		[id](Edge* e)
		{
			return e->getId() == id;
		}
	);
}

Token* Graph::getTokenById(Id id) const
{
	return findToken(
		[id](Token* t)
		{
			return t->getId() == id;
		}
	);
}

Node* Graph::createNodeHierarchy(const std::string& fullName)
{
	Node* node = getNode(fullName);
	if (node)
	{
		return node;
	}

	return insertNodeHierarchy(fullName);
}

Edge* Graph::createEdge(Edge::EdgeType type, Node* from, Node* to)
{
	Edge* edge = getEdge(type, from, to);
	if (edge)
	{
		return edge;
	}

	return insertEdge(type, from, to);
}

void Graph::removeNode(Node* node)
{
	std::vector<std::shared_ptr<Node> >::const_iterator it;
	for (it = m_nodes.begin(); it != m_nodes.end(); it++)
	{
		if (it->get() == node)
		{
			break;
		}
	}

	if (it == m_nodes.end())
	{
		LOG_WARNING("Node was not found in the graph.");
		return;
	}

	node->forEachEdgeOfType(Edge::EDGE_MEMBER, [this, node](Edge* e)
	{
		if (node == e->getFrom())
		{
			this->removeNode(e->getTo());
		}
	});

	node->forEachEdge(
		[this](Edge* e)
		{
			this->removeEdgeInternal(e);
		}
	);

	if (node->getEdges().size())
	{
		LOG_ERROR("Node has still edges.");
	}

	m_nodes.erase(it);
}

void Graph::removeEdge(Edge* edge)
{
	std::vector<std::shared_ptr<Edge> >::const_iterator it;
	for (it = m_edges.begin(); it != m_edges.end(); it++)
	{
		if (it->get() == edge)
		{
			break;
		}
	}

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
	std::vector<std::shared_ptr<Node> >::const_iterator it = find_if(m_nodes.begin(), m_nodes.end(),
		[&func](const std::shared_ptr<Node>& n)
		{
			return func(n.get());
		}
	);

	if (it != m_nodes.end())
	{
		return it->get();
	}

	return nullptr;
}

Edge* Graph::findEdge(std::function<bool(Edge*)> func) const
{
	std::vector<std::shared_ptr<Edge> >::const_iterator it = find_if(m_edges.begin(), m_edges.end(),
		[func](const std::shared_ptr<Edge>& e)
		{
			return func(e.get());
		}
	);

	if (it != m_edges.end())
	{
		return it->get();
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

void Graph::forEachNode(std::function<void(Node*)> func) const
{
	for (const std::shared_ptr<Node>& node : m_nodes)
	{
		func(node.get());
	}
}

void Graph::forEachEdge(std::function<void(Edge*)> func) const
{
	for (const std::shared_ptr<Edge>& edge : m_edges)
	{
		func(edge.get());
	}
}

void Graph::forEachToken(std::function<void(Token*)> func) const
{
	forEachNode(func);
	forEachEdge(func);
}

const std::vector<std::shared_ptr<Node> >& Graph::getNodes() const
{
	return m_nodes;
}

const std::vector<std::shared_ptr<Edge> >& Graph::getEdges() const
{
	return m_edges;
}

Node* Graph::addNodeAsPlainCopy(Node* node)
{
	Node* n = getNodeById(node->getId());
	if (n)
	{
		return n;
	}

	std::shared_ptr<Node> copy = node->createPlainCopy();
	m_nodes.push_back(copy);
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

	std::shared_ptr<Edge> copy = edge->createPlainCopy(from, to);
	m_edges.push_back(copy);
	return copy.get();
}

const std::string Graph::DELIMITER = "::";

Node* Graph::insertNodeHierarchy(const std::string& fullName)
{
	Node* node = nullptr;
	std::vector<std::string> names = utility::split(fullName, DELIMITER);
	std::string name;

	for (std::vector<std::string>::iterator it = names.begin(); it != names.end(); it++)
	{
		if (name.size() > 0)
		{
			name += DELIMITER;
		}
		name += *it;

		Node* childNode = getNode(name);

		if (!childNode)
		{
			std::shared_ptr<Node> childNodePtr = std::make_shared<Node>(Node::NODE_UNDEFINED, name);
			m_nodes.push_back(childNodePtr);
			childNode = childNodePtr.get();

			if (node)
			{
				createEdge(Edge::EDGE_MEMBER, node, childNode);
			}
		}

		node = childNode;
	}

	return node;
}

Edge* Graph::insertEdge(Edge::EdgeType type, Node* from, Node* to)
{
	std::shared_ptr<Edge> edgePtr = std::make_shared<Edge>(type, from, to);
	m_edges.push_back(edgePtr);
	return edgePtr.get();
}

void Graph::removeEdgeInternal(Edge* edge)
{
	std::vector<std::shared_ptr<Edge> >::const_iterator it;
	for (it = m_edges.begin(); it != m_edges.end(); it++)
	{
		if (it->get() == edge)
		{
			m_edges.erase(it);
			return;
		}
	}
}

std::ostream& operator<<(std::ostream& ostream, const Graph& graph)
{
	ostream << "Graph:\n";
	ostream << "nodes (" << graph.getNodes().size() << ")\n";
	graph.forEachNode(
		[&ostream](Node* n)
		{
			ostream << *n << '\n';
		}
	);

	ostream << "edges (" << graph.getEdges().size() << ")\n";
	graph.forEachEdge(
		[&ostream](Edge* e)
		{
			ostream << *e << '\n';
		}
	);

	return ostream;
}
