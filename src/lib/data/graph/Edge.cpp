#include "data/graph/Edge.h"

#include <sstream>

#include "data/graph/Node.h"
#include "data/graph/token_component/TokenComponentAccess.h"
#include "data/graph/token_component/TokenComponentDataType.h"
#include "utility/logging/logging.h"

Edge::Edge(EdgeType type, Node* from, Node* to)
	: m_type(type)
	, m_from(from)
	, m_to(to)
{
	m_from->addEdge(this);
	m_to->addEdge(this);

	checkType();
}

Edge::Edge(const Edge& other, Node* from, Node* to)
	: Token(other)
	, m_type(other.m_type)
	, m_from(from)
	, m_to(to)
{
	m_from->addEdge(this);
	m_to->addEdge(this);

	if (m_from == other.m_from || m_to == other.m_to ||
		m_from->getId() != other.m_from->getId() || m_to->getId() != other.m_to->getId())
	{
		LOG_ERROR("Nodes are not plain copies.");
	}

	checkType();
}

Edge::~Edge()
{
	m_from->removeEdge(this);
	m_to->removeEdge(this);
}

Edge::EdgeType Edge::getType() const
{
	return m_type;
}

bool Edge::isType(EdgeTypeMask mask) const
{
	return (m_type & mask) > 0;
}

Node* Edge::getFrom() const
{
	return m_from;
}

Node* Edge::getTo() const
{
	return m_to;
}

std::string Edge::getName() const
{
	return getTypeString() + ":" + getFrom()->getFullName() + "->" + getTo()->getFullName();
}

bool Edge::isNode() const
{
	return false;
}

bool Edge::isEdge() const
{
	return true;
}

void Edge::addComponentAccess(std::shared_ptr<TokenComponentAccess> component)
{
	if (getComponent<TokenComponentAccess>())
	{
		LOG_ERROR("TokenComponentAccess has been set before!");
	}
	else if (m_type != EDGE_MEMBER && m_type != EDGE_INHERITANCE)
	{
		LOG_ERROR("TokenComponentAccess can't be set on edge of type: " + getTypeString());
	}
	else
	{
		addComponent(component);
	}
}

void Edge::addComponentDataType(std::shared_ptr<TokenComponentDataType> component)
{
	if (getComponent<TokenComponentDataType>())
	{
		LOG_ERROR("TokenComponentDataType has been set before!");
	}
	else if (m_type != EDGE_TYPEDEF_OF && m_type != EDGE_TYPE_OF &&
		m_type != EDGE_RETURN_TYPE_OF && m_type != EDGE_PARAMETER_TYPE_OF &&
		m_type != EDGE_TYPE_USAGE)
	{
		LOG_ERROR("TokenComponentDataType can't be set on edge of type: " + getTypeString());
	}
	else
	{
		addComponent(component);
	}
}

std::string Edge::getTypeString(EdgeType type) const
{
	switch (type)
	{
	case EDGE_MEMBER:
		return "child";
	case EDGE_TYPE_OF:
		return "type_use";
	case EDGE_RETURN_TYPE_OF:
		return "return_type";
	case EDGE_PARAMETER_TYPE_OF:
		return "parameter_type";
	case EDGE_TYPE_USAGE:
		return "type_usage";
	case EDGE_INHERITANCE:
		return "inheritance";
	case EDGE_CALL:
		return "call";
	case EDGE_USAGE:
		return "usage";
	case EDGE_TYPEDEF_OF:
		return "typedef";
	}
	return "";
}

std::string Edge::getTypeString() const
{
	return getTypeString(m_type);
}

std::string Edge::getAsString() const
{
	std::stringstream str;
	str << "[" << getId() << "] " << getTypeString() << ": \"" << m_from->getName() << "\" -> \"" + m_to->getName() << "\"";

	TokenComponentAccess* component = getComponent<TokenComponentAccess>();
	if (component)
	{
		str << " " << component->getAccessString();
	}

	return str.str();
}

std::ostream& operator<<(std::ostream& ostream, const Edge& edge)
{
	ostream << edge.getAsString();
	return ostream;
}

bool Edge::checkType() const
{
	Node::NodeTypeMask typeMask = Node::NODE_UNDEFINED | Node::NODE_CLASS | Node::NODE_STRUCT | Node::NODE_ENUM | Node::NODE_TYPEDEF;
	Node::NodeTypeMask variableMask = Node::NODE_UNDEFINED | Node::NODE_GLOBAL_VARIABLE | Node::NODE_FIELD;
	Node::NodeTypeMask functionMask = Node::NODE_UNDEFINED_FUNCTION | Node::NODE_FUNCTION | Node::NODE_METHOD;

	switch (m_type)
	{
	case EDGE_MEMBER:
		if (!m_from->isType(Node::NODE_UNDEFINED | Node::NODE_CLASS | Node::NODE_STRUCT | Node::NODE_NAMESPACE | Node::NODE_ENUM) ||
			(m_to->isType(Node::NODE_NAMESPACE) && !m_from->isType(Node::NODE_UNDEFINED | Node::NODE_NAMESPACE)) ||
			(m_from->isType(Node::NODE_ENUM) && !m_to->isType(Node::NODE_FIELD)))
		{
			break;
		}
		return true;

	case EDGE_TYPE_OF:
		if (!m_from->isType(variableMask) || !m_to->isType(typeMask))
		{
			break;
		}
		return true;

	case EDGE_RETURN_TYPE_OF:
	case EDGE_PARAMETER_TYPE_OF:
	case EDGE_TYPE_USAGE:
		if (!m_from->isType(functionMask) || !m_to->isType(typeMask))
		{
			break;
		}
		return true;

	case EDGE_INHERITANCE:
		if (!m_from->isType(Node::NODE_CLASS) || !m_to->isType(Node::NODE_UNDEFINED | Node::NODE_CLASS))
		{
			break;
		}
		return true;

	case EDGE_CALL:
		if (!m_from->isType(variableMask | functionMask) || !m_to->isType(functionMask))
		{
			break;
		}
		return true;

	case EDGE_USAGE:
		if (!m_from->isType(functionMask) || !m_to->isType(variableMask))
		{
			break;
		}
		return true;

	case EDGE_TYPEDEF_OF:
		if (!m_from->isType(Node::NODE_TYPEDEF) || !m_to->isType(typeMask))
		{
			break;
		}
		return true;
	}

	LOG_ERROR_STREAM(
		<< "Edge " << getTypeString()
		<< " can't go from Node " << m_from->getTypeString()
		<< " to Node " << m_to->getTypeString()
	);
	return false;
}
