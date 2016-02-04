#include "data/graph/Node.h"

#include <sstream>

#include "utility/logging/logging.h"

#include "data/graph/token_component/TokenComponentAbstraction.h"
#include "data/graph/token_component/TokenComponentConst.h"
#include "data/graph/token_component/TokenComponentStatic.h"
#include "data/graph/token_component/TokenComponentFilePath.h"
#include "data/graph/token_component/TokenComponentSignature.h"

const Node::NodeTypeMask Node::NODE_NOT_VISIBLE = Node::NODE_UNDEFINED | Node::NODE_NAMESPACE;

std::string Node::getTypeString(NodeType type)
{
	switch (type)
	{
	case NODE_UNDEFINED:
		return "undefined";
	case NODE_TYPE:
		return "type";
	case NODE_NAMESPACE:
		return "namespace";
	case NODE_CLASS:
		return "class";
	case NODE_STRUCT:
		return "struct";
	case NODE_GLOBAL_VARIABLE:
		return "global_variable";
	case NODE_FIELD:
		return "field";
	case NODE_FUNCTION:
		return "function";
	case NODE_METHOD:
		return "method";
	case NODE_ENUM:
		return "enum";
	case NODE_ENUM_CONSTANT:
		return "enum_constant";
	case NODE_TYPEDEF:
		return "typedef";
	case NODE_TEMPLATE_PARAMETER_TYPE:
		return "template_parameter_type";
	case NODE_FILE:
		return "file";
	case NODE_MACRO:
		return "macro";
	}

	return "";
}

int Node::typeToInt(NodeType type)
{
	return type;
}

Node::NodeType Node::intToType(int value)
{
	switch (value)
	{
	case 0x2:
		return NODE_TYPE;
	case 0x4:
		return NODE_NAMESPACE;
	case 0x8:
		return NODE_STRUCT;
	case 0x10:
		return NODE_CLASS;
	case 0x20:
		return NODE_GLOBAL_VARIABLE;
	case 0x40:
		return NODE_FIELD;
	case 0x80:
		return NODE_FUNCTION;
	case 0x100:
		return NODE_METHOD;
	case 0x200:
		return NODE_ENUM;
	case 0x400:
		return NODE_ENUM_CONSTANT;
	case 0x800:
		return NODE_TYPEDEF;
	case 0x1000:
		return NODE_TEMPLATE_PARAMETER_TYPE;
	case 0x2000:
		return NODE_FILE;
	case 0x4000:
		return NODE_MACRO;
	}

	return NODE_UNDEFINED;
}

Node::Node(Id id, NodeType type, NameHierarchy nameHierarchy, bool defined)
	: Token(id)
	, m_type(type)
	, m_nameHierarchy(nameHierarchy)
	, m_defined(defined)
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
	if (!isType(type | NODE_UNDEFINED))
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

std::string Node::getName() const
{
	return m_nameHierarchy.getName();
}

std::string Node::getFullName() const
{
	return m_nameHierarchy.getFullName();
}

NameHierarchy Node::getNameHierarchy() const
{
	return m_nameHierarchy;
}

bool Node::isDefined() const
{
	return m_defined;
}

void Node::setDefined(bool defined)
{
	m_defined = defined;
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

Edge* Node::findEdgeOfType(Edge::EdgeTypeMask mask) const
{
	return findEdgeOfType(mask, [](Edge* e){ return true; });
}

Edge* Node::findEdgeOfType(Edge::EdgeTypeMask mask, std::function<bool(Edge*)> func) const
{
	std::vector<Edge*>::const_iterator it = find_if(m_edges.begin(), m_edges.end(),
		[mask, func](Edge* e)
		{
			if (e->isType(mask))
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

void Node::forEachEdgeOfType(Edge::EdgeTypeMask mask, std::function<void(Edge*)> func) const
{
	for_each(m_edges.begin(), m_edges.end(),
		[mask, func](Edge* e)
		{
			if (e->isType(mask))
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

void Node::forEachChildNodeRecursive(std::function<void(Node*)> func) const
{
	forEachEdgeOfType(Edge::EDGE_MEMBER,
		[func, this](Edge* e)
		{
			if (this != e->getTo())
			{
				func(e->getTo());
				e->getTo()->forEachChildNode(func);
			}
		}
	);
}

bool Node::hasReferences() const
{
	if (getLocationIds().size() > 0)
	{
		return true;
	}

	bool hasChildrenWithReferences = false;
	size_t childNodeCount = 0;

	forEachEdgeOfType(
		Edge::EDGE_MEMBER,
		[&](Edge* edge)
		{
			childNodeCount++;

			if (!hasChildrenWithReferences && edge->getTo() != this && edge->getTo()->hasReferences())
			{
				hasChildrenWithReferences = true;
			}
		}
	);

	if (hasChildrenWithReferences || getEdges().size() > childNodeCount)
	{
		return true;
	}

	return false;
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
		// LOG_ERROR("TokenComponentAbstraction has been set before!");
		return;
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
		// LOG_ERROR("TokenComponentConst has been set before!");
		return;
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
		// LOG_ERROR("TokenComponentStatic has been set before!");
		return;
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

void Node::addComponentFilePath(std::shared_ptr<TokenComponentFilePath> component)
{
	if (getComponent<TokenComponentFilePath>())
	{
		// LOG_ERROR("TokenComponentFilePath has been set before!");
		return;
	}
	else if (!isType(NODE_FILE))
	{
		LOG_ERROR("TokenComponentFilePath can't be set on node of type: " + getTypeString());
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
		return;
	}
	else if (!isType(NODE_FUNCTION | NODE_METHOD))
	{
		LOG_ERROR("TokenComponentFilePath can't be set on node of type: " + getTypeString());
	}
	else
	{
		addComponent(component);
	}
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
