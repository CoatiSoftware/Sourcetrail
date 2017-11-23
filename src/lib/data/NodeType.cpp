#include "data/NodeType.h"

#include "utility/ResourcePaths.h"
#include "utility/utilityString.h"

NodeType::NodeType(Type type)
	: m_type(type)
{
}

bool NodeType::operator==(const NodeType& o) const
{
	return m_type == o.m_type;
}

NodeType::Type NodeType::getType() const
{
	return m_type;
}

bool NodeType::isFile() const
{
	const NodeType::TypeMask mask =
		NodeType::NODE_FILE;
	return ((m_type & mask) > 0);
}

bool NodeType::isUnknownSymbol() const
{
	const NodeType::TypeMask mask =
		NodeType::NODE_SYMBOL;
	return ((m_type & mask) > 0);
}

bool NodeType::isInheritable() const
{
	// what about java enums?
	const NodeType::TypeMask mask =
		NodeType::NODE_SYMBOL | 
		NodeType::NODE_BUILTIN_TYPE | 
		NodeType::NODE_TYPE | 
		NodeType::NODE_STRUCT | 
		NodeType::NODE_CLASS | 
		NodeType::NODE_INTERFACE;

	return ((m_type & mask) > 0);
}

bool NodeType::isPackage() const
{
	const NodeType::TypeMask mask =
		NodeType::NODE_NAMESPACE |
		NodeType::NODE_PACKAGE;
	return ((m_type & mask) > 0);
}

bool NodeType::isCallable() const
{
	const NodeType::TypeMask mask =
		NodeType::NODE_FUNCTION |
		NodeType::NODE_METHOD;
	return ((m_type & mask) > 0);
}

bool NodeType::isVariable() const
{
	const NodeType::TypeMask mask =
		NodeType::NODE_GLOBAL_VARIABLE |
		NodeType::NODE_FIELD;
	return ((m_type & mask) > 0);
}

bool NodeType::isUsable() const
{
	const NodeType::TypeMask mask =
		NodeType::NODE_SYMBOL | 
		NodeType::NODE_BUILTIN_TYPE | 
		NodeType::NODE_STRUCT | 
		NodeType::NODE_CLASS | 
		NodeType::NODE_ENUM | 
		NodeType::NODE_UNION | 
		NodeType::NODE_INTERFACE |
		NodeType::NODE_TYPEDEF;
	return ((m_type & mask) > 0);
}

bool NodeType::isPotentialMember() const
{
	const NodeType::TypeMask mask =
		NodeType::NODE_METHOD | 
		NodeType::NODE_FIELD | 
		NodeType::NODE_CLASS | 
		NodeType::NODE_INTERFACE | 
		NodeType::NODE_STRUCT | 
		NodeType::NODE_UNION | 
		NodeType::NODE_TYPEDEF | 
		NodeType::NODE_ENUM;

	return ((m_type & mask) > 0);
}

bool NodeType::isCollapsible() const
{
	const NodeType::TypeMask mask =
		NodeType::NODE_SYMBOL | 
		NodeType::NODE_TYPE | 
		NodeType::NODE_BUILTIN_TYPE | 
		NodeType::NODE_CLASS | 
		NodeType::NODE_STRUCT | 
		NodeType::NODE_ENUM | 
		NodeType::NODE_UNION |
		NodeType::NODE_INTERFACE;
	return ((m_type & mask) > 0);
}

bool NodeType::isVisibleAsParentInGraph() const
{
	return !isPackage();
}

FilePath NodeType::getIconPath() const
{
	switch (m_type)
	{
	case NodeType::NODE_NAMESPACE:
	case NodeType::NODE_PACKAGE:
		// package icon cannot be changed
		return ResourcePaths::getGuiPath().concat(FilePath("graph_view/images/namespace.png"));
	case NodeType::NODE_ENUM:
		return ResourcePaths::getGuiPath().concat(FilePath("graph_view/images/enum.png"));
	case NodeType::NODE_TYPEDEF:
		return ResourcePaths::getGuiPath().concat(FilePath("graph_view/images/typedef.png"));
	case NodeType::NODE_MACRO:
		return ResourcePaths::getGuiPath().concat(FilePath("graph_view/images/macro.png"));
	case NodeType::NODE_FILE:
		return ResourcePaths::getGuiPath().concat(FilePath("graph_view/images/file.png"));
	default:
		return FilePath();
	}
}

bool NodeType::hasIcon() const
{
	const NodeType::TypeMask mask =
		NodeType::NODE_NAMESPACE |
		NodeType::NODE_PACKAGE |
		NodeType::NODE_ENUM |
		NodeType::NODE_TYPEDEF |
		NodeType::NODE_FILE |
		NodeType::NODE_MACRO;
	return ((m_type & mask) > 0);
}

NodeType::StyleType NodeType::getNodeStyle() const
{
	switch (m_type)
	{
	case NodeType::NODE_NAMESPACE:
	case NodeType::NODE_PACKAGE:
		return STYLE_PACKAGE;
	case NodeType::NODE_SYMBOL:
	case NodeType::NODE_TYPE:
	case NodeType::NODE_BUILTIN_TYPE:
	case NodeType::NODE_STRUCT:
	case NodeType::NODE_CLASS:
	case NodeType::NODE_UNION:
	case NodeType::NODE_INTERFACE:
	case NodeType::NODE_ENUM:
	case NodeType::NODE_TYPEDEF:
	case NodeType::NODE_TEMPLATE_PARAMETER_TYPE:
	case NodeType::NODE_TYPE_PARAMETER:
	case NodeType::NODE_FILE:
	case NodeType::NODE_MACRO:
		return STYLE_BIG_NODE;
	case NodeType::NODE_FUNCTION:
	case NodeType::NODE_METHOD:
	case NodeType::NODE_GLOBAL_VARIABLE:
	case NodeType::NODE_FIELD:
	case NodeType::NODE_ENUM_CONSTANT:
		return STYLE_SMALL_NODE;
	}
}

std::string NodeType::getUnderscoredTypeString() const
{
	return utility::replace(utility::replace(getReadableTypeString(), "-", "_"), " ", "_");
}

std::string NodeType::getReadableTypeString() const
{
	return utility::getReadableTypeString(m_type);
}

int utility::nodeTypeToInt(NodeType::Type type)
{
	return type;
}

NodeType::Type utility::intToType(int value)
{
	switch (value)
	{
	case NodeType::NODE_TYPE:
		return NodeType::NODE_TYPE;
	case NodeType::NODE_BUILTIN_TYPE:
		return NodeType::NODE_BUILTIN_TYPE;
	case NodeType::NODE_NAMESPACE:
		return NodeType::NODE_NAMESPACE;
	case NodeType::NODE_PACKAGE:
		return NodeType::NODE_PACKAGE;
	case NodeType::NODE_STRUCT:
		return NodeType::NODE_STRUCT;
	case NodeType::NODE_CLASS:
		return NodeType::NODE_CLASS;
	case NodeType::NODE_INTERFACE:
		return NodeType::NODE_INTERFACE;
	case NodeType::NODE_GLOBAL_VARIABLE:
		return NodeType::NODE_GLOBAL_VARIABLE;
	case NodeType::NODE_FIELD:
		return NodeType::NODE_FIELD;
	case NodeType::NODE_FUNCTION:
		return NodeType::NODE_FUNCTION;
	case NodeType::NODE_METHOD:
		return NodeType::NODE_METHOD;
	case NodeType::NODE_ENUM:
		return NodeType::NODE_ENUM;
	case NodeType::NODE_ENUM_CONSTANT:
		return NodeType::NODE_ENUM_CONSTANT;
	case NodeType::NODE_TYPEDEF:
		return NodeType::NODE_TYPEDEF;
	case NodeType::NODE_TEMPLATE_PARAMETER_TYPE:
		return NodeType::NODE_TEMPLATE_PARAMETER_TYPE;
	case NodeType::NODE_TYPE_PARAMETER:
		return NodeType::NODE_TYPE_PARAMETER;
	case NodeType::NODE_FILE:
		return NodeType::NODE_FILE;
	case NodeType::NODE_MACRO:
		return NodeType::NODE_MACRO;
	case NodeType::NODE_UNION:
		return NodeType::NODE_UNION;
	}

	return NodeType::NODE_SYMBOL;
}

std::string utility::getReadableTypeString(NodeType::Type type)
{
	switch (type)
	{
	case NodeType::NODE_SYMBOL:
		return "symbol";
	case NodeType::NODE_BUILTIN_TYPE:
		return "built-in type";
	case NodeType::NODE_TYPE:
		return "type";
	case NodeType::NODE_NAMESPACE:
		return "namespace";
	case NodeType::NODE_PACKAGE:
		return "package";
	case NodeType::NODE_STRUCT:
		return "struct";
	case NodeType::NODE_CLASS:
		return "class";
	case NodeType::NODE_INTERFACE:
		return "interface";
	case NodeType::NODE_GLOBAL_VARIABLE:
		return "global variable";
	case NodeType::NODE_FIELD:
		return "field";
	case NodeType::NODE_FUNCTION:
		return "function";
	case NodeType::NODE_METHOD:
		return "method";
	case NodeType::NODE_ENUM:
		return "enum";
	case NodeType::NODE_ENUM_CONSTANT:
		return "enum constant";
	case NodeType::NODE_TYPEDEF:
		return "typedef";
	case NodeType::NODE_TEMPLATE_PARAMETER_TYPE:
		return "template parameter type";
	case NodeType::NODE_TYPE_PARAMETER:
		return "type parameter";
	case NodeType::NODE_FILE:
		return "file";
	case NodeType::NODE_MACRO:
		return "macro";
	case NodeType::NODE_UNION:
		return "union";
	}

	return "";
}

NodeType::Type utility::getTypeForReadableTypeString(const std::string str)
{
	for (NodeType::TypeMask mask = 1; mask <= NodeType::NODE_MAX_VALUE; mask *= 2)
	{
		NodeType::Type type = intToType(mask);
		if (getReadableTypeString(type) == str)
		{
			return type;
		}
	}

	return NodeType::NODE_SYMBOL;
}