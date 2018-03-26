#ifndef EDGE_H
#define EDGE_H

#include <memory>
#include <string>

#include "data/graph/Token.h"

class Node;

class Edge
	: public Token
{
public:
	typedef int TypeMask;
	enum EdgeType : TypeMask
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
	bool isType(TypeMask mask) const;

	Node* getFrom() const;
	Node* getTo() const;

	std::wstring getName() const;

	// Token implementation
	virtual bool isNode() const override;
	virtual bool isEdge() const override;

	static std::wstring getUnderscoredTypeString(EdgeType type);
	static std::wstring getReadableTypeString(EdgeType type);
	// Logging.
	virtual std::wstring getReadableTypeString() const override;
	std::wstring getAsString() const;

private:
	void operator=(const Node&);

	bool checkType() const;

	const EdgeType m_type;

	Node* const m_from;
	Node* const m_to;
};

std::wostream& operator<<(std::wostream& ostream, const Edge& edge);

#endif // EDGE_H
