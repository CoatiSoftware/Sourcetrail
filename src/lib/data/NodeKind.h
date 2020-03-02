#ifndef NODE_KIND_H
#define NODE_KIND_H

#include <string>

typedef int NodeKindMask;

enum NodeKind : NodeKindMask
{	 // make sure that the value of 0x0 is not used here because it doesn't work for bitmasking.
	NODE_SYMBOL = 1 << 0,
	NODE_TYPE = 1 << 1,
	NODE_BUILTIN_TYPE = 1 << 2,

	NODE_MODULE = 1 << 3,
	NODE_NAMESPACE = 1 << 4,
	NODE_PACKAGE = 1 << 5,
	NODE_STRUCT = 1 << 6,
	NODE_CLASS = 1 << 7,
	NODE_INTERFACE = 1 << 8,
	NODE_ANNOTATION = 1 << 9,
	NODE_GLOBAL_VARIABLE = 1 << 10,
	NODE_FIELD = 1 << 11,
	NODE_FUNCTION = 1 << 12,
	NODE_METHOD = 1 << 13,
	NODE_ENUM = 1 << 14,
	NODE_ENUM_CONSTANT = 1 << 15,
	NODE_TYPEDEF = 1 << 16,
	NODE_TYPE_PARAMETER = 1 << 17,

	NODE_FILE = 1 << 18,
	NODE_MACRO = 1 << 19,
	NODE_UNION = 1 << 20,

	NODE_MAX_VALUE = NODE_UNION
};

int nodeKindToInt(NodeKind kind);
NodeKind intToNodeKind(int value);

std::string getReadableNodeKindString(NodeKind kind);
std::wstring getReadableNodeKindWString(NodeKind kind);
NodeKind getNodeKindForReadableNodeKindString(const std::wstring& str);

#endif	  // NODE_KIND_H
