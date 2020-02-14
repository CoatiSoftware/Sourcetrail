#include "Node.h"

#include <sstream>

#include "logging.h"
#include "utilityString.h"

#include "TokenComponentAccess.h"
#include "TokenComponentConst.h"
#include "TokenComponentStatic.h"

Node::Node(Id id, NodeType type, NameHierarchy nameHierarchy, DefinitionKind definitionKind)
	: Token(id)
	, m_type(type)
	, m_nameHierarchy(std::move(nameHierarchy))
	, m_definitionKind(definitionKind)
	, m_childCount(0)
{
}

Node::Node(const Node& other)
	: Token(other)
	, m_type(other.m_type)
	, m_nameHierarchy(other.m_nameHierarchy)
	, m_definitionKind(other.m_definitionKind)
	, m_childCount(other.m_childCount)
{
}

Node::~Node() {}

NodeType Node::getType() const
{
	return m_type;
}

void Node::setType(NodeType type)
{
	if (!isType(type.getKind() | NODE_SYMBOL))
	{
		LOG_WARNING(
			L"Cannot change NodeType after it was already set from " + getReadableTypeString() +
			L" to " + type.getReadableTypeWString());
		return;
	}
	m_type = type;
}

bool Node::isType(NodeKindMask mask) const
{
	return (m_type.getKind() & mask) > 0;
}

std::wstring Node::getName() const
{
	return m_nameHierarchy.getRawName();
}

std::wstring Node::getFullName() const
{
	return m_nameHierarchy.getQualifiedName();
}

const NameHierarchy& Node::getNameHierarchy() const
{
	return m_nameHierarchy;
}

bool Node::isDefined() const
{
	return m_definitionKind != DEFINITION_NONE;
}

bool Node::isImplicit() const
{
	return m_definitionKind == DEFINITION_IMPLICIT;
}

bool Node::isExplicit() const
{
	return m_definitionKind == DEFINITION_EXPLICIT;
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
	return findEdgeOfType(Edge::EDGE_MEMBER, [this](Edge* e) { return e->getTo() == this; });
}

bool Node::isParentOf(const Node* node) const
{
	while ((node = node->getParentNode()))
	{
		if (node == this)
		{
			return true;
		}
	}
	return false;
}

Edge* Node::findEdge(std::function<bool(Edge*)> func) const
{
	auto it = find_if(
		m_edges.begin(), m_edges.end(), [func](std::pair<Id, Edge*> p) { return func(p.second); });

	if (it != m_edges.end())
	{
		return it->second;
	}

	return nullptr;
}

Edge* Node::findEdgeOfType(Edge::TypeMask mask) const
{
	return findEdgeOfType(mask, [](Edge* e) { return true; });
}

Edge* Node::findEdgeOfType(Edge::TypeMask mask, std::function<bool(Edge*)> func) const
{
	auto it = find_if(m_edges.begin(), m_edges.end(), [mask, func](std::pair<Id, Edge*> p) {
		if (p.second->isType(mask))
		{
			return func(p.second);
		}
		return false;
	});

	if (it != m_edges.end())
	{
		return it->second;
	}

	return nullptr;
}

Node* Node::findChildNode(std::function<bool(Node*)> func) const
{
	auto it = find_if(m_edges.begin(), m_edges.end(), [&func](std::pair<Id, Edge*> p) {
		if (p.second->getType() == Edge::EDGE_MEMBER)
		{
			return func(p.second->getTo());
		}
		return false;
	});

	if (it != m_edges.end())
	{
		return it->second->getTo();
	}

	return nullptr;
}

void Node::forEachEdge(std::function<void(Edge*)> func) const
{
	for_each(m_edges.begin(), m_edges.end(), [func](std::pair<Id, Edge*> p) { func(p.second); });
}

void Node::forEachEdgeOfType(Edge::TypeMask mask, std::function<void(Edge*)> func) const
{
	for_each(m_edges.begin(), m_edges.end(), [mask, func](std::pair<Id, Edge*> p) {
		if (p.second->isType(mask))
		{
			func(p.second);
		}
	});
}

void Node::forEachChildNode(std::function<void(Node*)> func) const
{
	forEachEdgeOfType(Edge::EDGE_MEMBER, [func, this](Edge* e) {
		if (this != e->getTo())
		{
			func(e->getTo());
		}
	});
}

void Node::forEachNodeRecursive(std::function<void(const Node*)> func) const
{
	func(this);

	forEachEdgeOfType(Edge::EDGE_MEMBER, [func, this](Edge* e) {
		if (this != e->getTo())
		{
			e->getTo()->forEachNodeRecursive(func);
		}
	});
}

bool Node::isNode() const
{
	return true;
}

bool Node::isEdge() const
{
	return false;
}

std::wstring Node::getReadableTypeString() const
{
	return m_type.getReadableTypeWString();
}

std::wstring Node::getAsString() const
{
	std::wstringstream str;
	str << L"[" << getId() << L"] " << getReadableTypeString() << L": " << L"\"" << getName()
		<< L"\"";

	TokenComponentAccess* access = getComponent<TokenComponentAccess>();
	if (access)
	{
		str << L" " << access->getAccessString();
	}

	if (getComponent<TokenComponentStatic>())
	{
		str << L" static";
	}

	if (getComponent<TokenComponentConst>())
	{
		str << L" const";
	}

	return str.str();
}

std::wostream& operator<<(std::wostream& ostream, const Node& node)
{
	ostream << node.getAsString();
	return ostream;
}
