#include "data/graph/Edge.h"

#include <sstream>

#include "data/graph/Node.h"
#include "data/graph/token_component/TokenComponentAggregation.h"
#include "data/graph/token_component/TokenComponentInheritanceChain.h"
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
	case EDGE_MEMBER:
		return EDGE_MEMBER;
	case EDGE_TYPE_USAGE:
		return EDGE_TYPE_USAGE;
	case EDGE_USAGE:
		return EDGE_USAGE;
	case EDGE_CALL:
		return EDGE_CALL;
	case EDGE_INHERITANCE:
		return EDGE_INHERITANCE;
	case EDGE_OVERRIDE:
		return EDGE_OVERRIDE;
	case EDGE_TEMPLATE_ARGUMENT:
		return EDGE_TEMPLATE_ARGUMENT;
	case EDGE_TYPE_ARGUMENT:
		return EDGE_TYPE_ARGUMENT;
	case EDGE_TEMPLATE_DEFAULT_ARGUMENT:
		return EDGE_TEMPLATE_DEFAULT_ARGUMENT;
	case EDGE_TEMPLATE_SPECIALIZATION:
		return EDGE_TEMPLATE_SPECIALIZATION;
	case EDGE_TEMPLATE_MEMBER_SPECIALIZATION:
		return EDGE_TEMPLATE_MEMBER_SPECIALIZATION;
	case EDGE_INCLUDE:
		return EDGE_INCLUDE;
	case EDGE_IMPORT:
		return EDGE_IMPORT;
	case EDGE_AGGREGATION:
		return EDGE_AGGREGATION;
	case EDGE_MACRO_USAGE:
		return EDGE_MACRO_USAGE;
	}

	return EDGE_UNDEFINED;
}

Edge::Edge(Id id, EdgeType type, Node* from, Node* to)
	: Token(id)
	, m_type(type)
	, m_from(from)
	, m_to(to)
{
	m_from->addEdge(this);
	m_to->addEdge(this);
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
	return getReadableTypeString() + ":" + getFrom()->getFullName() + "->" + getTo()->getFullName();
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
		LOG_ERROR("TokenComponentAggregation can't be set on edge of type: " + getReadableTypeString());
	}
	else
	{
		addComponent(component);
	}
}

void Edge::addComponentInheritanceChain(std::shared_ptr<TokenComponentInheritanceChain> component)
{
	if (getComponent<TokenComponentInheritanceChain>())
	{
		LOG_ERROR("TokenComponentInheritanceChain has been set before!");
	}
	else if (m_type != EDGE_INHERITANCE)
	{
		LOG_ERROR("TokenComponentInheritanceChain can't be set on edge of type: " + getReadableTypeString());
	}
	else
	{
		addComponent(component);
	}
}

std::string Edge::getUnderscoredTypeString(EdgeType type)
{
	return utility::replace(utility::replace(getReadableTypeString(type), "-", "_"), " ", "_");
}

std::string Edge::getReadableTypeString(EdgeType type)
{
	switch (type)
	{
	case EDGE_UNDEFINED:
		return "undefined";
	case EDGE_MEMBER:
		return "child";
	case EDGE_TYPE_USAGE:
		return "type use";
	case EDGE_USAGE:
		return "use";
	case EDGE_CALL:
		return "call";
	case EDGE_INHERITANCE:
		return "inheritance";
	case EDGE_OVERRIDE:
		return "override";
	case EDGE_TEMPLATE_ARGUMENT:
		return "template argument";
	case EDGE_TYPE_ARGUMENT:
		return "type argument";
	case EDGE_TEMPLATE_DEFAULT_ARGUMENT:
		return "template default argument";
	case EDGE_TEMPLATE_SPECIALIZATION:
		return "template specialization";
	case EDGE_TEMPLATE_MEMBER_SPECIALIZATION:
		return "template member specialization";
	case EDGE_INCLUDE:
		return "include";
	case EDGE_IMPORT:
		return "import";
	case EDGE_AGGREGATION:
		return "aggregation";
	case EDGE_MACRO_USAGE:
		return "macro use";
	}

	return "";
}

std::string Edge::getReadableTypeString() const
{
	return getReadableTypeString(m_type);
}

std::string Edge::getAsString() const
{
	std::stringstream str;
	str << "[" << getId() << "] " << getReadableTypeString() << ": \"" << m_from->getName() << "\" -> \"" + m_to->getName() << "\"";

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
