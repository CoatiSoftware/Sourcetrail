#include "data/graph/Node.h"

#include <sstream>

// #include "data/graph/edgeComponent/EdgeComponentDataType.h"
// #include "data/type/DataType.h"
#include "utility/logging/logging.h"

Node::Node(NodeType type, const std::string& name)
	: m_type(type)
	, m_name(name)
	, m_isConst(false)
	, m_isStatic(false)
{
}

Node::~Node()
{
}

std::shared_ptr<Node> Node::createPlainCopy() const
{
	std::shared_ptr<Node> node(new Node(getId(), m_type, m_name));

	node->setConst(m_isConst);
	node->setStatic(m_isStatic);

	return node;
}

Node::NodeType Node::getType() const
{
	return m_type;
}

void Node::setType(NodeType type)
{
	if (type != m_type && m_type != NODE_NAMESPACE && m_type != NODE_UNDEFINED)
	{
		LOG_WARNING(
			"Changing NodeType after it was already set, from " + getTypeString(m_type) + " to " + getTypeString(type)
		);
	}
	m_type = type;
}

const std::string& Node::getName() const
{
	return m_name;
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

	return NULL;
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

	return NULL;
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

bool Node::isNode() const
{
	return true;
}

bool Node::isEdge() const
{
	return false;
}

void Node::setAccess(Edge::AccessType access)
{
	if (access != Edge::ACCESS_NONE)
	{
		if (!getMemberEdge())
		{
			LOG_WARNING("Can't set access on node " + getName() + ", because it is not a child.");
			return;
		}
		getMemberEdge()->setAccess(access);
	}
}

bool Node::isConst() const
{
	return m_isConst;
}

void Node::setConst(bool isConst)
{
	if (isConst && m_type != NODE_GLOBAL_VARIABLE && m_type != NODE_FIELD && m_type != NODE_METHOD)
	{
		LOG_ERROR("Setting const on wrong node of type " + getTypeString(m_type));
		return;
	}

	m_isConst = isConst;
}

bool Node::isStatic() const
{
	return m_isStatic;
}

void Node::setStatic(bool isStatic)
{
	if (isStatic && m_type != NODE_GLOBAL_VARIABLE && m_type != NODE_FIELD && m_type != NODE_METHOD)
	{
		LOG_ERROR("Setting static on wrong node of type " + getTypeString(m_type));
		return;
	}

	m_isStatic = isStatic;
}

std::string Node::getSignature() const
{
	// Signature generation from edges failed, because parameter edges to same type are bundled.
	// std::string str;

	// Edge* returnTypeEdge = findEdgeOfType(Edge::EDGE_RETURN_TYPE_OF);
	// str += returnTypeEdge->getComponent<EdgeComponentDataType>()->getDataType().getFullTypeName() + " " + m_name + "(";

	// forEachEdgeOfType(Edge::EDGE_PARAMETER_TYPE_OF,
	// 	[&str](Edge* edge)
	// 	{
	// 		str += edge->getComponent<EdgeComponentDataType>()->getDataType().getFullTypeName() + ", ";
	// 	}
	// );

	// if (*str.rbegin() == ' ')
	// {
	// 	str.pop_back();
	// 	str.pop_back();
	// }
	// str += ")";

	// if (isConst())
	// {
	// 	str += " const";
	// }

	// return str;

	return m_signature;
}

void Node::setSignature(const std::string& signature)
{
	if (m_type != NODE_FUNCTION && m_type != NODE_METHOD && m_type != NODE_UNDEFINED)
	{
		LOG_ERROR("Signature is not supported on node of type " + getTypeString(m_type));
		return;
	}

	if (m_signature.size())
	{
		LOG_ERROR("Signature was already set before.");
		return;
	}

	m_signature = signature;
}

std::string Node::getTypeString(NodeType type) const
{
	switch (type)
	{
	case NODE_UNDEFINED:
		return "type";
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

std::string Node::getAsString() const
{
	std::stringstream str;
	str << "[" << getId() << "] " << getTypeString(m_type) << ": " << "\"" << getName() << "\"";

	if (isStatic())
	{
		str << " static";
	}

	if (isConst())
	{
		str << " const";
	}

	return str.str();
}

Node::Node(Id id, NodeType type, const std::string& name)
	: Token(id)
	, m_type(type)
	, m_name(name)
	, m_isConst(false)
	, m_isStatic(false)
{
}

std::ostream& operator<<(std::ostream& ostream, const Node& node)
{
	ostream << node.getAsString();
	return ostream;
}
