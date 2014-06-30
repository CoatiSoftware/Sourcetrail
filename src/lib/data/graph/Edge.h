#ifndef EDGE_H
#define EDGE_H

#include <memory>
#include <string>

#include "data/graph/Token.h"

class Node;

class Edge: public Token
{
public:
	enum EdgeType
	{
		EDGE_MEMBER,
		EDGE_TYPE_OF,
		EDGE_RETURN_TYPE_OF,
		EDGE_PARAMETER_OF,
		EDGE_USAGE,
		EDGE_CALL,
		EDGE_INHERITANCE
	};

	enum AccessType
	{
		ACCESS_PUBLIC,
		ACCESS_PROTECTED,
		ACCESS_PRIVATE,
		ACCESS_NONE
	};

	Edge(EdgeType type, Node* from, Node* to);
	virtual ~Edge();

	std::shared_ptr<Edge> createPlainCopy(Node* from, Node* to) const;

	EdgeType getType() const;

	Node* getFrom() const;
	Node* getTo() const;

	// Token implementation
	virtual bool isNode() const;
	virtual bool isEdge() const;

	// Additional field accessors for different EdgeTypes.
	AccessType getAccess() const;
	void setAccess(AccessType access);

	// Logging.
	std::string getTypeString() const;
	std::string getAccessString() const;
	std::string getAsString() const;

private:
	// Constructor for plain copies.
	Edge(Id id, EdgeType type, Node* from, Node* to);

	const EdgeType m_type;

	Node* const m_from;
	Node* const m_to;

	// Additional fields for different EdgeTypes.
	AccessType m_access;
};

std::ostream& operator<<(std::ostream& ostream, const Edge& edge);

#endif // EDGE_H
