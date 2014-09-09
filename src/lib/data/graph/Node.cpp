#include "data/graph/Node.h"

#include <sstream>

#include "data/graph/token_component/TokenComponentAbstraction.h"
#include "data/graph/token_component/TokenComponentConst.h"
#include "data/graph/token_component/TokenComponentName.h"
#include "data/graph/token_component/TokenComponentStatic.h"
#include "data/graph/token_component/TokenComponentSignature.h"
#include "utility/logging/logging.h"

Node::Node(NodeType type, const std::string& name)
	: m_type(type)
	, m_nameComponent(std::make_shared<TokenComponentNameCached>(name))
{
}

Node::Node(NodeType type, std::shared_ptr<TokenComponentName> nameComponent)
	: m_type(type)
	, m_nameComponent(nameComponent)
{
}

Node::Node(const Node& other)
	: Token(other)
	, m_type(other.m_type)
	, m_nameComponent(other.m_nameComponent->copyComponentName())
{
}

Node::~Node()
{
}

Node::NodeType Node::getType() const
{
	return m_type;
}

void Node::setType(NodeType type)
{
	if (!isType(type | NODE_UNDEFINED | NODE_UNDEFINED_FUNCTION | NODE_UNDEFINED_VARIABLE | NODE_UNDEFINED_TYPE))
	{
		LOG_WARNING(
			"Cannot change NodeType after it was already set from " + getTypeString() + " to " + getTypeString(type)
		);
		return;
	}
	m_type = type;
}

bool Node::isType(NodeTypeMask mask) const
{
	return (m_type & mask) > 0;
}

const std::string& Node::getName() const
{
	return m_nameComponent->getName();
}

std::string Node::getFullName() const
{
	return m_nameComponent->getFullName();
}

const std::vector<Edge*>& Node::getEdges() const
{
	return m_edges;
}

void Node::addEdge(Edge* edge)
{
	m_edges.push_back(edge);
}

void Node::removeEdge(Edge* edge)
{
	m_edges.erase(find(m_edges.begin(), m_edges.end(), edge));
}

Node* Node::getParentNode() const
{
	Edge* edge = getMemberEdge();
	if (edge)
	{
		return edge->getFrom();
	}
	return nullptr;
}

Node* Node::getLastParentNode()
{
	Node* parent = getParentNode();
	if (parent)
	{
		return parent->getLastParentNode();
	}
	return this;
}

Edge* Node::getMemberEdge() const
{
	return findEdgeOfType(Edge::EDGE_MEMBER,
		[this](Edge* e)
		{
			return e->getTo() == this;
		}
	);
}

Edge* Node::findEdge(std::function<bool(Edge*)> func) const
{
	std::vector<Edge*>::const_iterator it = find_if(m_edges.begin(), m_edges.end(), func);

	if (it != m_edges.end())
	{
		return *it;
	}

	return nullptr;
}

Edge* Node::findEdgeOfType(Edge::EdgeType type) const
{
	return findEdgeOfType(type, [](Edge* e){ return true; });
}

Edge* Node::findEdgeOfType(Edge::EdgeType type, std::function<bool(Edge*)> func) const
{
	std::vector<Edge*>::const_iterator it = find_if(m_edges.begin(), m_edges.end(),
		[type, func](Edge* e)
		{
			if (e->getType() == type)
			{
				return func(e);
			}
			return false;
		}
	);

	if (it != m_edges.end())
	{
		return *it;
	}

	return nullptr;
}

Node* Node::findChildNode(std::function<bool(Node*)> func) const
{
	std::vector<Edge*>::const_iterator it = find_if(m_edges.begin(), m_edges.end(),
		[&func](Edge* e)
		{
			if (e->getType() == Edge::EDGE_MEMBER)
			{
				return func(e->getTo());
			}
			return false;
		}
	);

	if (it != m_edges.end())
	{
		return (*it)->getTo();
	}

	return nullptr;
}

void Node::forEachEdge(std::function<void(Edge*)> func) const
{
	for_each(m_edges.begin(), m_edges.end(), func);
}

void Node::forEachEdgeOfType(Edge::EdgeType type, std::function<void(Edge*)> func) const
{
	for_each(m_edges.begin(), m_edges.end(),
		[type, func](Edge* e)
		{
			if (e->getType() == type)
			{
				func(e);
			}
		}
	);
}

void Node::forEachChildNode(std::function<void(Node*)> func) const
{
	forEachEdgeOfType(Edge::EDGE_MEMBER,
		[func, this](Edge* e)
		{
			if (this != e->getTo())
			{
				func(e->getTo());
			}
		}
	);
}

bool Node::isNode() const
{
	return true;
}

bool Node::isEdge() const
{
	return false;
}

void Node::addComponentAbstraction(std::shared_ptr<TokenComponentAbstraction> component)
{
	if (getComponent<TokenComponentAbstraction>())
	{
		LOG_ERROR("TokenComponentAbstraction has been set before!");
	}
	else if (!isType(NODE_METHOD))
	{
		LOG_ERROR("TokenComponentAbstraction can't be set on node of type: " + getTypeString());
	}
	else
	{
		addComponent(component);
	}
}

void Node::addComponentConst(std::shared_ptr<TokenComponentConst> component)
{
	if (getComponent<TokenComponentConst>())
	{
		LOG_ERROR("TokenComponentConst has been set before!");
	}
	else if (!isType(NODE_METHOD))
	{
		LOG_ERROR("TokenComponentConst can't be set on node of type: " + getTypeString());
	}
	else
	{
		addComponent(component);
	}
}

void Node::addComponentStatic(std::shared_ptr<TokenComponentStatic> component)
{
	if (getComponent<TokenComponentStatic>())
	{
		LOG_ERROR("TokenComponentStatic has been set before!");
	}
	else if (!isType(NODE_GLOBAL_VARIABLE | NODE_FIELD | NODE_FUNCTION | NODE_METHOD))
	{
		LOG_ERROR("TokenComponentStatic can't be set on node of type: " + getTypeString());
	}
	else
	{
		addComponent(component);
	}
}

void Node::addComponentSignature(std::shared_ptr<TokenComponentSignature> component)
{
	if (getComponent<TokenComponentSignature>())
	{
		LOG_ERROR("TokenComponentSignature has been set before!");
	}
	else if (!isType(NODE_UNDEFINED_FUNCTION | NODE_FUNCTION | NODE_METHOD))
	{
		LOG_ERROR("TokenComponentSignature can't be set on node of type: " + getTypeString());
	}
	else
	{
		addComponent(component);
	}
}

std::string Node::getTypeString(NodeType type) const
{
	switch (type)
	{
	case NODE_UNDEFINED:
		return "undefined";
	case NODE_UNDEFINED_FUNCTION:
		return "undefined_function";
	case NODE_UNDEFINED_VARIABLE:
		return "undefined_variable";
	case NODE_UNDEFINED_TYPE:
		return "undefined_type";
	case NODE_CLASS:
		return "class";
	case NODE_STRUCT:
		return "struct";
	case NODE_GLOBAL_VARIABLE:
		return "global";
	case NODE_FIELD:
		return "field";
	case NODE_FUNCTION:
		return "function";
	case NODE_METHOD:
		return "method";
	case NODE_NAMESPACE:
		return "namespace";
	case NODE_ENUM:
		return "enum";
	case NODE_TYPEDEF:
		return "typedef";
	}
	return "";
}

std::string Node::getTypeString() const
{
	return getTypeString(m_type);
}

std::string Node::getAsString() const
{
	std::stringstream str;
	str << "[" << getId() << "] " << getTypeString() << ": " << "\"" << getName() << "\"";

	if (getComponent<TokenComponentStatic>())
	{
		str << " static";
	}

	if (getComponent<TokenComponentConst>())
	{
		str << " const";
	}

	return str.str();
}

std::ostream& operator<<(std::ostream& ostream, const Node& node)
{
	ostream << node.getAsString();
	return ostream;
}
