#ifndef EDGE_H
#define EDGE_H

#include <memory>
#include <string>

#include "data/graph/Token.h"

class Node;
class TokenComponentAccess;
class TokenComponentDataType;

class Edge: public Token
{
public:
	enum EdgeType
	{
		EDGE_MEMBER,
		EDGE_TYPE_OF,
		EDGE_RETURN_TYPE_OF,
		EDGE_PARAMETER_TYPE_OF,
		EDGE_TYPE_USAGE,
		EDGE_USAGE,
		EDGE_CALL,
		EDGE_INHERITANCE,
		EDGE_TYPEDEF_OF
	};

	Edge(EdgeType type, Node* from, Node* to);
	Edge(const Edge& other, Node* from, Node* to);
	virtual ~Edge();

	EdgeType getType() const;

	Node* getFrom() const;
	Node* getTo() const;

	std::string getName() const;

	// Token implementation
	virtual bool isNode() const;
	virtual bool isEdge() const;

	// Component setters
	void addComponentAccess(std::shared_ptr<TokenComponentAccess> component);
	void addComponentDataType(std::shared_ptr<TokenComponentDataType> component);

	// Logging.
	std::string getTypeString() const;
	std::string getAsString() const;

private:
	void operator=(const Node&);

	const EdgeType m_type;

	Node* const m_from;
	Node* const m_to;
};

std::ostream& operator<<(std::ostream& ostream, const Edge& edge);

#endif // EDGE_H
