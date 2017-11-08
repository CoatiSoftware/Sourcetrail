#include "data/graph/Node.h"

#include <sstream>

#include "utility/logging/logging.h"
#include "utility/utilityString.h"

#include "data/graph/token_component/TokenComponentAbstraction.h"
#include "data/graph/token_component/TokenComponentAccess.h"
#include "data/graph/token_component/TokenComponentConst.h"
#include "data/graph/token_component/TokenComponentStatic.h"
#include "data/graph/token_component/TokenComponentFilePath.h"

const Node::NodeTypeMask Node::NODE_NOT_VISIBLE =
	NODE_NAMESPACE | NODE_PACKAGE;

const Node::NodeTypeMask Node::NODE_USEABLE_TYPE =
	NODE_NON_INDEXED | NODE_BUILTIN_TYPE | NODE_STRUCT | NODE_CLASS | NODE_ENUM | NODE_UNION | NODE_INTERFACE |
	NODE_TYPEDEF;

const Node::NodeTypeMask Node::NODE_INHERITABLE_TYPE =
	NODE_NON_INDEXED | NODE_BUILTIN_TYPE | NODE_TYPE | NODE_STRUCT | NODE_CLASS | NODE_INTERFACE;

const Node::NodeTypeMask Node::NODE_MEMBER_TYPE =
	NODE_METHOD | NODE_FIELD | NODE_CLASS | NODE_INTERFACE | NODE_STRUCT | NODE_UNION | NODE_TYPEDEF | NODE_ENUM;

const Node::NodeTypeMask Node::NODE_COLLAPSIBLE_TYPE =
	NODE_NON_INDEXED | NODE_TYPE | NODE_BUILTIN_TYPE | NODE_CLASS | NODE_STRUCT | NODE_ENUM | NODE_UNION |
	NODE_INTERFACE;

std::string Node::getUnderscoredTypeString(NodeType type)
{
	return utility::replace(utility::replace(getReadableTypeString(type), "-", "_"), " ", "_");
}

std::string Node::getReadableTypeString(NodeType type)
{
	switch (type)
	{
	case NODE_NON_INDEXED:
		return "non-indexed";
	case NODE_BUILTIN_TYPE:
		return "built-in type";
	case NODE_TYPE:
		return "type";
	case NODE_NAMESPACE:
		return "namespace";
	case NODE_PACKAGE:
		return "package";
	case NODE_STRUCT:
		return "struct";
	case NODE_CLASS:
		return "class";
	case NODE_INTERFACE:
		return "interface";
	case NODE_GLOBAL_VARIABLE:
		return "global variable";
	case NODE_FIELD:
		return "field";
	case NODE_FUNCTION:
		return "function";
	case NODE_METHOD:
		return "method";
	case NODE_ENUM:
		return "enum";
	case NODE_ENUM_CONSTANT:
		return "enum constant";
	case NODE_TYPEDEF:
		return "typedef";
	case NODE_TEMPLATE_PARAMETER_TYPE:
		return "template parameter type";
	case NODE_TYPE_PARAMETER:
		return "type parameter";
	case NODE_FILE:
		return "file";
	case NODE_MACRO:
		return "macro";
	case NODE_UNION:
		return "union";
	}

	return "";
}

Node::NodeType Node::getTypeForReadableTypeString(const std::string str)
{
	for (Node::NodeTypeMask mask = 1; mask <= NODE_MAX_VALUE; mask *= 2)
	{
		Node::NodeType type = intToType(mask);
		if (getReadableTypeString(type) == str)
		{
			return type;
		}
	}

	return NODE_NON_INDEXED;
}

int Node::typeToInt(NodeType type)
{
	return type;
}

Node::NodeType Node::intToType(int value)
{
	switch (value)
	{
	case NODE_TYPE:
		return NODE_TYPE;
	case NODE_BUILTIN_TYPE:
		return NODE_BUILTIN_TYPE;
	case NODE_NAMESPACE:
		return NODE_NAMESPACE;
	case NODE_PACKAGE:
		return NODE_PACKAGE;
	case NODE_STRUCT:
		return NODE_STRUCT;
	case NODE_CLASS:
		return NODE_CLASS;
	case NODE_INTERFACE:
		return NODE_INTERFACE;
	case NODE_GLOBAL_VARIABLE:
		return NODE_GLOBAL_VARIABLE;
	case NODE_FIELD:
		return NODE_FIELD;
	case NODE_FUNCTION:
		return NODE_FUNCTION;
	case NODE_METHOD:
		return NODE_METHOD;
	case NODE_ENUM:
		return NODE_ENUM;
	case NODE_ENUM_CONSTANT:
		return NODE_ENUM_CONSTANT;
	case NODE_TYPEDEF:
		return NODE_TYPEDEF;
	case NODE_TEMPLATE_PARAMETER_TYPE:
		return NODE_TEMPLATE_PARAMETER_TYPE;
	case NODE_TYPE_PARAMETER:
		return NODE_TYPE_PARAMETER;
	case NODE_FILE:
		return NODE_FILE;
	case NODE_MACRO:
		return NODE_MACRO;
	case NODE_UNION:
		return NODE_UNION;
	}

	return NODE_NON_INDEXED;
}

Node::Node(Id id, NodeType type, NameHierarchy nameHierarchy, bool defined)
	: Token(id)
	, m_type(type)
	, m_nameHierarchy(nameHierarchy)
	, m_defined(defined)
	, m_implicit(false)
	, m_explicit(false)
	, m_childCount(0)
{
}

Node::Node(const Node& other)
	: Token(other)
	, m_type(other.m_type)
	, m_nameHierarchy(other.m_nameHierarchy)
	, m_defined(other.m_defined)
	, m_implicit(other.m_implicit)
	, m_explicit(other.m_explicit)
	, m_childCount(other.m_childCount)
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
	if (!isType(type | NODE_NON_INDEXED))
	{
		LOG_WARNING(
			"Cannot change NodeType after it was already set from " + getReadableTypeString() + " to " + getReadableTypeString(type)
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
	return m_nameHierarchy.getRawName();
}

std::string Node::getFullName() const
{
	return m_nameHierarchy.getQualifiedName();
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

bool Node::isImplicit() const
{
	return m_implicit;
}

void Node::setImplicit(bool implicit)
{
	m_implicit = implicit;
}

bool Node::isExplicit() const
{
	return m_explicit;
}

void Node::setExplicit(bool bExplicit)
{
	m_explicit = bExplicit;
}

size_t Node::getChildCount() const
{
	return m_childCount;
}

void Node::setChildCount(size_t childCount)
{
	m_childCount = childCount;
}

size_t Node::getEdgeCount() const
{
	return m_edges.size();
}

void Node::addEdge(Edge* edge)
{
	m_edges.emplace(edge->getId(), edge);
}

void Node::removeEdge(Edge* edge)
{
	auto it = m_edges.find(edge->getId());
	if (it != m_edges.end())
	{
		m_edges.erase(it);
	}
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
	auto it = find_if(m_edges.begin(), m_edges.end(),
		[func](std::pair<Id, Edge*> p)
		{
			return func(p.second);
		}
	);

	if (it != m_edges.end())
	{
		return it->second;
	}

	return nullptr;
}

Edge* Node::findEdgeOfType(Edge::EdgeTypeMask mask) const
{
	return findEdgeOfType(mask, [](Edge* e){ return true; });
}

Edge* Node::findEdgeOfType(Edge::EdgeTypeMask mask, std::function<bool(Edge*)> func) const
{
	auto it = find_if(m_edges.begin(), m_edges.end(),
		[mask, func](std::pair<Id, Edge*> p)
		{
			if (p.second->isType(mask))
			{
				return func(p.second);
			}
			return false;
		}
	);

	if (it != m_edges.end())
	{
		return it->second;
	}

	return nullptr;
}

Node* Node::findChildNode(std::function<bool(Node*)> func) const
{
	auto it = find_if(m_edges.begin(), m_edges.end(),
		[&func](std::pair<Id, Edge*> p)
		{
			if (p.second->getType() == Edge::EDGE_MEMBER)
			{
				return func(p.second->getTo());
			}
			return false;
		}
	);

	if (it != m_edges.end())
	{
		return it->second->getTo();
	}

	return nullptr;
}

void Node::forEachEdge(std::function<void(Edge*)> func) const
{
	for_each(m_edges.begin(), m_edges.end(),
		[func](std::pair<Id, Edge*> p)
		{
			func(p.second);
		}
	);
}

void Node::forEachEdgeOfType(Edge::EdgeTypeMask mask, std::function<void(Edge*)> func) const
{
	for_each(m_edges.begin(), m_edges.end(),
		[mask, func](std::pair<Id, Edge*> p)
		{
			if (p.second->isType(mask))
			{
				func(p.second);
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

void Node::forEachNodeRecursive(std::function<void(const Node*)> func) const
{
	func(this);

	forEachEdgeOfType(Edge::EDGE_MEMBER,
		[func, this](Edge* e)
		{
			if (this != e->getTo())
			{
				e->getTo()->forEachNodeRecursive(func);
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
		// LOG_ERROR("TokenComponentAbstraction has been set before!");
		return;
	}
	else if (!isType(NODE_METHOD))
	{
		LOG_ERROR("TokenComponentAbstraction can't be set on node of type: " + getReadableTypeString());
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
		LOG_ERROR("TokenComponentConst can't be set on node of type: " + getReadableTypeString());
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
		LOG_ERROR("TokenComponentStatic can't be set on node of type: " + getReadableTypeString());
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
		LOG_ERROR("TokenComponentFilePath can't be set on node of type: " + getReadableTypeString());
	}
	else
	{
		addComponent(component);
	}
}

void Node::addComponentAccess(std::shared_ptr<TokenComponentAccess> component)
{
	if (getComponent<TokenComponentAccess>())
	{
		LOG_WARNING("TokenComponentAccess has been set before!");
		return;
	}
	else
	{
		addComponent(component);
	}
}

std::string Node::getReadableTypeString() const
{
	return getReadableTypeString(m_type);
}

std::string Node::getAsString() const
{
	std::stringstream str;
	str << "[" << getId() << "] " << getReadableTypeString() << ": " << "\"" << getName() << "\"";

	TokenComponentAccess* access = getComponent<TokenComponentAccess>();
	if (access)
	{
		str << " " << access->getAccessString();
	}

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
