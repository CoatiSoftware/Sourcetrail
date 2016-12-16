#ifndef EDGE_H
#define EDGE_H

#include <memory>
#include <string>

#include "data/graph/Token.h"

class Node;
class TokenComponentAggregation;
class TokenComponentAccess;
class TokenComponentDataType;

class Edge
	: public Token
{
public:
	typedef int EdgeTypeMask;
	enum EdgeType : EdgeTypeMask
	{
		EDGE_UNDEFINED							= 0x0,
		EDGE_MEMBER								= 0x1,
		EDGE_TYPE_USAGE							= 0x2,
		EDGE_USAGE								= 0x4,
		EDGE_CALL								= 0x8,
		EDGE_INHERITANCE						= 0x10,
		EDGE_OVERRIDE							= 0x20,
		EDGE_TEMPLATE_ARGUMENT					= 0x40,
		EDGE_TYPE_ARGUMENT						= 0x80,
		EDGE_TEMPLATE_DEFAULT_ARGUMENT			= 0x100,
		EDGE_TEMPLATE_SPECIALIZATION			= 0x200,
		EDGE_TEMPLATE_MEMBER_SPECIALIZATION		= 0x400,
		EDGE_INCLUDE							= 0x800,
		EDGE_IMPORT								= 0x1000,
		EDGE_AGGREGATION						= 0x2000,
		EDGE_MACRO_USAGE						= 0x4000
	};

	static int typeToInt(EdgeType type);
	static EdgeType intToType(int value);

	Edge(Id id, EdgeType type, Node* from, Node* to);
	Edge(const Edge& other, Node* from, Node* to);
	virtual ~Edge();

	EdgeType getType() const;
	bool isType(EdgeTypeMask mask) const;

	Node* getFrom() const;
	Node* getTo() const;

	std::string getName() const;

	// Token implementation
	virtual bool isNode() const;
	virtual bool isEdge() const;

	// Component setters
	void addComponentAggregation(std::shared_ptr<TokenComponentAggregation> component);

	// Logging.
	static std::string getTypeString(EdgeType type);
	virtual std::string getTypeString() const;
	std::string getAsString() const;

private:
	void operator=(const Node&);

	bool checkType() const;

	const EdgeType m_type;

	Node* const m_from;
	Node* const m_to;
};

std::ostream& operator<<(std::ostream& ostream, const Edge& edge);

#endif // EDGE_H
