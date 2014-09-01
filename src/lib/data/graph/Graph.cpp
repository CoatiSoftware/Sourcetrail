#include "data/graph/Graph.h"

#include "data/graph/token_component/TokenComponentSignature.h"
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

Graph& Graph::operator=(const Graph& other)
{
	if (&other != this)
	{
		other.forEachNode(
			[this](Node* node)
			{
				addNodeAsPlainCopy(node);
			}
		);

		other.forEachEdge(
			[this](Edge* edge)
			{
				addEdgeAsPlainCopy(edge);
			}
		);
	}

	return *this;
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

const std::map<Id, std::shared_ptr<Node>>& Graph::getNodes() const
{
	return m_nodes;
}

const std::map<Id, std::shared_ptr<Edge>>& Graph::getEdges() const
{
	return m_edges;
}

Node* Graph::getNode(const std::string& fullName) const
{
	std::deque<std::string> names = utility::split<std::deque<std::string>>(fullName, DELIMITER);
	Node* node = getLastValidNode(&names);

	if (node && !names.size())
	{
		return node;
	}

	return nullptr;
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

Token* Graph::getTokenById(Id id) const
{
	Token* token = getNodeById(id);
	if (!token)
	{
		token = getEdgeById(id);
	}
	return token;
}

Node* Graph::createNodeHierarchy(const std::string& fullName)
{
	return createNodeHierarchy(Node::NODE_UNDEFINED, fullName);
}

Node* Graph::createNodeHierarchy(Node::NodeType type, const std::string& fullName)
{
	std::deque<std::string> names = utility::split<std::deque<std::string>>(fullName, DELIMITER);
	Node* node = getLastValidNode(&names);
	if (node && !names.size())
	{
		if (type != Node::NODE_UNDEFINED)
		{
			node->setType(type);
		}
		return node;
	}

	return insertNodeHierarchy(type, names, node);
}

Node* Graph::createNodeHierarchyWithDistinctSignature(const std::string& fullName, const std::string& signature)
{
	return createNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, fullName, signature);
}

Node* Graph::createNodeHierarchyWithDistinctSignature(
	Node::NodeType type, const std::string& fullName, const std::string& signature
){
	std::deque<std::string> names = utility::split<std::deque<std::string>>(fullName, DELIMITER);
	Node* node = getLastValidNode(&names);
	if (node && !names.size())
	{
		TokenComponentSignature* sigComponent = node->getComponent<TokenComponentSignature>();
		if (sigComponent && sigComponent->getSignature() == signature)
		{
			if (type != Node::NODE_UNDEFINED && type != Node::NODE_UNDEFINED_FUNCTION)
			{
				node->setType(type);
			}
			return node;
		}

		Node* parentNode = node->getParentNode();
		const std::string& name = node->getName();

		std::function<bool(Node*)> findSignature =
			[&name, &signature](Node* n)
			{
				TokenComponentSignature* c = n->getComponent<TokenComponentSignature>();
				return n->getName() == name && c && c->getSignature() == signature;
			};

		if (parentNode)
		{
			node = parentNode->findChildNode(findSignature);
		}
		else
		{
			node = findNode(findSignature);
		}

		if (node)
		{
			return node;
		}

		node = insertNode(type, name, parentNode);
	}
	else
	{
		node = insertNodeHierarchy(type, names, node);
	}

	node->addComponentSignature(std::make_shared<TokenComponentSignature>(signature));
	return node;
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
	std::map<Id, std::shared_ptr<Node>>::const_iterator it = m_nodes.find(node->getId());
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

const std::string Graph::DELIMITER = "::";

Node* Graph::getLastValidNode(std::deque<std::string>* names) const
{
	const std::string& name = names->front();

	Node* node = findNode(
		[&name](Node* n)
		{
			return n->getName() == name && n->getParentNode() == nullptr;
		}
	);

	if (!node)
	{
		return nullptr;
	}

	names->pop_front();

	while (names->size())
	{
		const std::string& name = names->front();
		Node* childNode = node->findChildNode(
			[&name](Node* n)
			{
				return n->getName() == name;
			}
		);

		if (!childNode)
		{
			break;
		}

		node = childNode;
		names->pop_front();
	}

	return node;
}

Node* Graph::insertNodeHierarchy(Node::NodeType type, std::deque<std::string> names, Node* parentNode)
{
	while (names.size())
	{
		parentNode = insertNode(names.size() == 1 ? type : Node::NODE_UNDEFINED, names.front(), parentNode);
		names.pop_front();
	}

	return parentNode;
}

Node* Graph::insertNode(Node::NodeType type, const std::string& name, Node* parentNode)
{
	std::shared_ptr<Node> nodePtr = std::make_shared<Node>(type, name);
	m_nodes.emplace(nodePtr->getId(), nodePtr);

	Node* node = nodePtr.get();

	if (parentNode)
	{
		createEdge(Edge::EDGE_MEMBER, parentNode, node);
	}

	return node;
}

Edge* Graph::insertEdge(Edge::EdgeType type, Node* from, Node* to)
{
	std::shared_ptr<Edge> edgePtr = std::make_shared<Edge>(type, from, to);
	m_edges.emplace(edgePtr->getId(), edgePtr);
	return edgePtr.get();
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
