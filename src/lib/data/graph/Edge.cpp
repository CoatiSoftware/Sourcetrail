#include "data/graph/Edge.h"

#include <sstream>

#include "data/graph/Node.h"
#include "data/graph/token_component/TokenComponentAggregation.h"
#include "data/graph/token_component/TokenComponentAccess.h"
#include "utility/logging/logging.h"
#include "utility/utilityString.h"

int Edge::typeToInt(EdgeType type)
{
	return type;
}

Edge::EdgeType Edge::intToType(int value)
{
	switch (value)
	{
	case 0x1:
		return EDGE_MEMBER;
	case 0x2:
		return EDGE_TYPE_OF;
	case 0x4:
		return EDGE_RETURN_TYPE_OF;
	case 0x8:
		return EDGE_PARAMETER_TYPE_OF;
	case 0x10:
		return EDGE_TYPE_USAGE;
	case 0x20:
		return EDGE_USAGE;
	case 0x40:
		return EDGE_CALL;
	case 0x80:
		return EDGE_INHERITANCE;
	case 0x100:
		return EDGE_OVERRIDE;
	case 0x200:
		return EDGE_TYPEDEF_OF;
	case 0x400:
		return EDGE_TEMPLATE_PARAMETER_OF;
	case 0x800:
		return EDGE_TEMPLATE_ARGUMENT_OF;
	case 0x1000:
		return EDGE_TEMPLATE_DEFAULT_ARGUMENT_OF;
	case 0x2000:
		return EDGE_TEMPLATE_SPECIALIZATION_OF;
	case 0x4000:
		return EDGE_INCLUDE;
	case 0x8000:
		return EDGE_AGGREGATION;
	}
}

Edge::Edge(EdgeType type, Node* from, Node* to)
	: m_type(type)
	, m_from(from)
	, m_to(to)
{
	m_from->addEdge(this);
	m_to->addEdge(this);

	checkType();
}

Edge::Edge(Id id, EdgeType type, Node* from, Node* to)
	: Token(id)
	, m_type(type)
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

void Edge::splitName(const std::string& name, EdgeType* type, std::string* fromName, std::string* toName)
{
	EdgeTypeMask mask = 1;
	std::string typeStr = utility::substrBefore(name, ':');

	while (typeStr != getTypeString(static_cast<EdgeType>(mask)))
	{
		mask = mask << 1;
	}

	*type = static_cast<EdgeType>(mask);

	std::string names = utility::substrAfter(name, ':');

	*fromName = utility::substrBefore(names, '-');
	*toName = utility::substrAfter(utility::substrAfter(names, '-'), '>');
}

bool Edge::isNode() const
{
	return false;
}

bool Edge::isEdge() const
{
	return true;
}

void Edge::addComponentAggregation(std::shared_ptr<TokenComponentAggregation> component)
{
	if (getComponent<TokenComponentAggregation>())
	{
		LOG_ERROR("TokenComponentAggregation has been set before!");
	}
	else if (m_type != EDGE_AGGREGATION)
	{
		LOG_ERROR("TokenComponentAggregation can't be set on edge of type: " + getTypeString());
	}
	else
	{
		addComponent(component);
	}
}

void Edge::addComponentAccess(std::shared_ptr<TokenComponentAccess> component)
{
	if (getComponent<TokenComponentAccess>())
	{
		// LOG_ERROR("TokenComponentAccess has been set before!");
		return;
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

std::string Edge::getTypeString(EdgeType type)
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
	case EDGE_OVERRIDE:
		return "override";
	case EDGE_CALL:
		return "call";
	case EDGE_USAGE:
		return "usage";
	case EDGE_TYPEDEF_OF:
		return "typedef";
	case EDGE_TEMPLATE_PARAMETER_OF:
		return "template_parameter";
	case EDGE_TEMPLATE_ARGUMENT_OF:
		return "template_argument";
	case EDGE_TEMPLATE_DEFAULT_ARGUMENT_OF:
		return "template_default_argument";
	case EDGE_TEMPLATE_SPECIALIZATION_OF:
		return "template_specialization";
	case EDGE_INCLUDE:
		return "include";
	case EDGE_AGGREGATION:
		return "aggregation";
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

	TokenComponentAccess* access = getComponent<TokenComponentAccess>();
	if (access)
	{
		str << " " << access->getAccessString();
	}

	TokenComponentAggregation* aggregation = getComponent<TokenComponentAggregation>();
	if (aggregation)
	{
		str << " " << aggregation->getAggregationCount();
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
	Node::NodeTypeMask complexTypeMask = Node::NODE_UNDEFINED_TYPE | Node::NODE_CLASS | Node::NODE_STRUCT | Node:: NODE_TEMPLATE_PARAMETER_TYPE;
	Node::NodeTypeMask typeMask = Node::NODE_UNDEFINED | Node::NODE_ENUM | Node::NODE_TYPEDEF | complexTypeMask;
	Node::NodeTypeMask variableMask = Node::NODE_UNDEFINED | Node::NODE_UNDEFINED_VARIABLE | Node::NODE_GLOBAL_VARIABLE | Node::NODE_FIELD;
	Node::NodeTypeMask functionMask = Node::NODE_UNDEFINED_FUNCTION | Node::NODE_FUNCTION | Node::NODE_METHOD;

	switch (m_type)
	{
	case EDGE_MEMBER:
		if (!m_from->isType(typeMask | Node::NODE_NAMESPACE | functionMask) ||
			(!m_from->isType(Node::NODE_UNDEFINED | Node::NODE_NAMESPACE) && m_to->isType(Node::NODE_NAMESPACE)) ||
			(m_from->isType(Node::NODE_ENUM) && !m_to->isType(Node::NODE_ENUM_CONSTANT)) ||
			(m_from->isType(functionMask) && !m_to->isType(Node::NODE_TEMPLATE_PARAMETER_TYPE)))
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
		if (!m_from->isType(complexTypeMask) || !m_to->isType(complexTypeMask))
		{
			break;
		}
		return true;

	case EDGE_OVERRIDE:
		if (!m_from->isType(Node::NODE_UNDEFINED_FUNCTION | Node::NODE_METHOD) ||
			!m_to->isType(Node::NODE_UNDEFINED_FUNCTION | Node::NODE_METHOD))
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
		if (!m_from->isType(functionMask) || !m_to->isType(variableMask | Node::NODE_ENUM_CONSTANT))
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

	case EDGE_TEMPLATE_PARAMETER_OF:
		if (!m_from->isType(Node::NODE_TEMPLATE_PARAMETER_TYPE) || !m_to->isType(typeMask | functionMask))
		{
			break;
		}
		return true;

	case EDGE_TEMPLATE_ARGUMENT_OF:
	case EDGE_TEMPLATE_DEFAULT_ARGUMENT_OF:
		if (!m_from->isType(typeMask) || !m_to->isType(typeMask | functionMask))
		{
			break;
		}
		return true;

	case EDGE_TEMPLATE_SPECIALIZATION_OF:
		if (!m_from->isType(typeMask | functionMask) || !m_to->isType(typeMask | functionMask))
		{
			break;
		}
		return true;

	case EDGE_INCLUDE:
		if (!m_from->isType(Node::NODE_FILE) || !m_to->isType(Node::NODE_FILE))
		{
			break;
		}
		return true;

	case EDGE_AGGREGATION:
		if (!m_from->isType(typeMask | variableMask | functionMask) || !m_to->isType(typeMask | variableMask | functionMask))
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
