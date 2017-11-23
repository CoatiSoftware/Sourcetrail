#ifndef NODE_TYPE_H
#define NODE_TYPE_H

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "utility/types.h"

class NodeType
{
public:
	typedef int NodeType::TypeMask;
	enum Type : NodeType::TypeMask
	{ // make sure that the value of 0x0 is not used here because it doesn't work for bitmasking.
		NODE_NON_INDEXED = 0x1,
		NODE_TYPE = 0x2,
		NODE_BUILTIN_TYPE = 0x4,

		NODE_NAMESPACE = 0x8,
		NODE_PACKAGE = 0x10,
		NODE_STRUCT = 0x20,
		NODE_CLASS = 0x40,
		NODE_INTERFACE = 0x80,
		NODE_GLOBAL_VARIABLE = 0x100,
		NODE_FIELD = 0x200,
		NODE_FUNCTION = 0x400,
		NODE_METHOD = 0x800,

		NODE_ENUM = 0x1000,
		NODE_ENUM_CONSTANT = 0x2000,
		NODE_TYPEDEF = 0x4000,
		NODE_TEMPLATE_PARAMETER_TYPE = 0x8000,
		NODE_TYPE_PARAMETER = 0x10000,

		NODE_FILE = 0x20000,
		NODE_MACRO = 0x40000,
		NODE_UNION = 0x80000,

		NODE_MAX_VALUE = NODE_UNION
	};

	NodeType(Type type);

	bool operator==(const NodeType& o) const;

	Type getType() const;

	bool isFile() const;
	bool isInheritable() const;
	bool isPackage() const;
	bool isCallable() const;
	bool isUsable() const;
	bool isPotentialMember() const;
	bool isCollapsible() const;
	bool isVisibleAsParentInGraph() const;

	int getFontSizeOffset() const;

	std::string getUnderscoredTypeString() const;
	std::string getReadableTypeString() const;

private:
	Type m_type;
};

namespace utility
{
	int nodeTypeToInt(NodeType::Type type);
	NodeType::Type intToType(int value);
	std::string getReadableTypeString(NodeType::Type type);
	NodeType::Type getTypeForReadableTypeString(const std::string str);
}

#endif // NODE_TYPE_H
