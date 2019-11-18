#ifndef EDGE_H
#define EDGE_H

#include <memory>
#include <string>

#include "Token.h"

class Node;

class Edge: public Token
{
public:
	typedef int TypeMask;
	enum EdgeType : TypeMask
	{
		EDGE_UNDEFINED = 0,
		EDGE_MEMBER = 1 << 0,
		EDGE_TYPE_USAGE = 1 << 1,
		EDGE_USAGE = 1 << 2,
		EDGE_CALL = 1 << 3,
		EDGE_INHERITANCE = 1 << 4,
		EDGE_OVERRIDE = 1 << 5,
		EDGE_TYPE_ARGUMENT = 1 << 6,
		EDGE_TEMPLATE_SPECIALIZATION = 1 << 7,
		EDGE_INCLUDE = 1 << 8,
		EDGE_IMPORT = 1 << 9,
		EDGE_AGGREGATION = 1 << 10,
		EDGE_MACRO_USAGE = 1 << 11,
		EDGE_ANNOTATION_USAGE = 1 << 12,

		EDGE_MAX_VALUE = EDGE_ANNOTATION_USAGE
	};

	static int typeToInt(EdgeType type);
	static EdgeType intToType(int value);

	static const TypeMask LAYOUT_VERTICAL = EDGE_INHERITANCE | EDGE_OVERRIDE |
		EDGE_TEMPLATE_SPECIALIZATION;

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
	static EdgeType getTypeForReadableTypeString(const std::wstring& str);

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

#endif	  // EDGE_H
