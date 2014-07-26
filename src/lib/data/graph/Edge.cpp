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

Node* Edge::getFrom() const
{
	return m_from;
}

Node* Edge::getTo() const
{
	return m_to;
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
	else if (m_type != EDGE_TYPEDEF_OF && m_type != EDGE_TYPE_OF
		&& m_type != EDGE_RETURN_TYPE_OF && m_type != EDGE_PARAMETER_TYPE_OF)
	{
		LOG_ERROR("TokenComponentDataType can't be set on edge of type: " + getTypeString());
	}
	else
	{
		addComponent(component);
	}
}

std::string Edge::getTypeString() const
{
	switch (m_type)
	{
	case EDGE_MEMBER:
		return "has child";
	case EDGE_TYPE_OF:
		return "is type of";
	case EDGE_RETURN_TYPE_OF:
		return "has return type";
	case EDGE_PARAMETER_TYPE_OF:
		return "has parameter of type";
	case EDGE_INHERITANCE:
		return "is derived from";
	case EDGE_CALL:
		return "calls";
	case EDGE_USAGE:
		return "uses";
	case EDGE_TYPEDEF_OF:
		return "is typedef of";
	}
	return "";
}

std::string Edge::getAsString() const
{
	std::stringstream str;
	str << "[" << getId() << "] \"" << m_from->getName() << "\" " << getTypeString() << " \"" + m_to->getName() << "\"";

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
