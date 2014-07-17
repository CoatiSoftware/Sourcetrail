#include "data/graph/Edge.h"

#include <sstream>

#include "data/graph/edgeComponent/EdgeComponent.h"
#include "data/graph/Node.h"
#include "utility/logging/logging.h"

Edge::Edge(EdgeType type, Node* from, Node* to)
	: m_type(type)
	, m_from(from)
	, m_to(to)
	, m_access(ACCESS_NONE)
{
	m_from->addEdge(this);
	m_to->addEdge(this);
}

Edge::~Edge()
{
	m_from->removeEdge(this);
	m_to->removeEdge(this);
}

std::shared_ptr<Edge> Edge::createPlainCopy(Node* from, Node* to) const
{
	if (from == m_from || to == m_to || from->getId() != m_from->getId() || to->getId() != m_to->getId())
	{
		LOG_ERROR("Nodes are not plain copies.");
		return nullptr;
	}

	std::shared_ptr<Edge> edge(new Edge(getId(), m_type, from, to));

	edge->setAccess(m_access);

	for (std::shared_ptr<EdgeComponent> component: m_components)
	{
		edge->addComponent(component->copy());
	}

	return edge;
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

Edge::AccessType Edge::getAccess() const
{
	return m_access;
}

void Edge::setAccess(AccessType access)
{
	if (access != ACCESS_NONE && m_type != EDGE_MEMBER && m_type != EDGE_INHERITANCE)
	{
		LOG_ERROR("Setting access on wrong edge type.");
		return;
	}

	if (m_access != ACCESS_NONE && access != m_access)
	{
		LOG_WARNING("Different AccessType was already set before.");
	}
	m_access = access;
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
	default:
		LOG_ERROR("TypeString not implemented for edge type.");
	}
	return "";
}

std::string Edge::getAccessString() const
{
	switch (m_access)
	{
	case ACCESS_PUBLIC:
		return "public";
	case ACCESS_PROTECTED:
		return "protected";
	case ACCESS_PRIVATE:
		return "private";
	case ACCESS_NONE:
		return "";
	}
	return "";
}

std::string Edge::getAsString() const
{
	std::stringstream str;
	str << "[" << getId() << "] \"" << m_from->getName() << "\" " << getTypeString() << " \"" + m_to->getName() << "\"";

	if (m_access != ACCESS_NONE)
	{
		str << " " << getAccessString();
	}

	return str.str();
}

void Edge::addComponent(std::shared_ptr<EdgeComponent> component)
{
	m_components.push_back(component);
	component->setEdge(this);
}

Edge::Edge(Id id, EdgeType type, Node* from, Node* to)
	: Token(id)
	, m_type(type)
	, m_from(from)
	, m_to(to)
	, m_access(ACCESS_NONE)
{
	m_from->addEdge(this);
	m_to->addEdge(this);
}

std::ostream& operator<<(std::ostream& ostream, const Edge& edge)
{
	ostream << edge.getAsString();
	return ostream;
}
