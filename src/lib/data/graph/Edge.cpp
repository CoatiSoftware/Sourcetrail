#include "Edge.h"

#include <sstream>

#include "Node.h"
#include "TokenComponentAggregation.h"
#include "logging.h"
#include "utilityString.h"

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
	case EDGE_ANNOTATION_USAGE:
		return EDGE_ANNOTATION_USAGE;
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

bool Edge::isType(TypeMask mask) const
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

std::wstring Edge::getName() const
{
	return getReadableTypeString() + L":" + getFrom()->getFullName() + L"->" + getTo()->getFullName();
}

bool Edge::isNode() const
{
	return false;
}

bool Edge::isEdge() const
{
	return true;
}

std::wstring Edge::getUnderscoredTypeString(EdgeType type)
{
	return utility::replace(utility::replace(getReadableTypeString(type), L"-", L"_"), L" ", L"_");
}

std::wstring Edge::getReadableTypeString(EdgeType type)
{
	switch (type)
	{
	case EDGE_UNDEFINED:
		return L"undefined";
	case EDGE_MEMBER:
		return L"child";
	case EDGE_TYPE_USAGE:
		return L"type use";
	case EDGE_USAGE:
		return L"use";
	case EDGE_CALL:
		return L"call";
	case EDGE_INHERITANCE:
		return L"inheritance";
	case EDGE_OVERRIDE:
		return L"override";
	case EDGE_TEMPLATE_ARGUMENT:
		return L"template argument";
	case EDGE_TYPE_ARGUMENT:
		return L"type argument";
	case EDGE_TEMPLATE_DEFAULT_ARGUMENT:
		return L"template default argument";
	case EDGE_TEMPLATE_SPECIALIZATION:
		return L"template specialization";
	case EDGE_TEMPLATE_MEMBER_SPECIALIZATION:
		return L"template member specialization";
	case EDGE_INCLUDE:
		return L"include";
	case EDGE_IMPORT:
		return L"import";
	case EDGE_AGGREGATION:
		return L"aggregation";
	case EDGE_MACRO_USAGE:
		return L"macro use";
	case EDGE_ANNOTATION_USAGE:
		return L"annotation use";
	}

	return L"";
}

Edge::EdgeType Edge::getTypeForReadableTypeString(const std::wstring& str)
{
	for (TypeMask mask = 1; mask <= EDGE_MAX_VALUE; mask *= 2)
	{
		EdgeType type = intToType(mask);
		if (getReadableTypeString(type) == str)
		{
			return type;
		}
	}

	return EDGE_UNDEFINED;
}

std::wstring Edge::getReadableTypeString() const
{
	return getReadableTypeString(m_type);
}

std::wstring Edge::getAsString() const
{
	std::wstringstream str;
	str << L"[" << getId() << L"] " << getReadableTypeString();
	str << L": \"" << m_from->getName() << L"\" -> \"" + m_to->getName() << L"\"";

	TokenComponentAggregation* aggregation = getComponent<TokenComponentAggregation>();
	if (aggregation)
	{
		str << L" " << aggregation->getAggregationCount();
	}

	return str.str();
}

std::wostream& operator<<(std::wostream& ostream, const Edge& edge)
{
	ostream << edge.getAsString();
	return ostream;
}
