#include "NodeType.h"

#include "ResourcePaths.h"
#include "utilityString.h"

std::vector<NodeType> const NodeType::overviewBundleNodeTypesOrdered = 
{
	NodeType(NodeType::NODE_FILE),
	NodeType(NodeType::NODE_MACRO),
	NodeType(NodeType::NODE_ANNOTATION),
	NodeType(NodeType::NODE_MODULE),
	NodeType(NodeType::NODE_NAMESPACE),
	NodeType(NodeType::NODE_PACKAGE),
	NodeType(NodeType::NODE_CLASS),
	NodeType(NodeType::NODE_INTERFACE),
	NodeType(NodeType::NODE_STRUCT),
	NodeType(NodeType::NODE_UNION),
	NodeType(NodeType::NODE_FUNCTION),
	NodeType(NodeType::NODE_GLOBAL_VARIABLE),
	NodeType(NodeType::NODE_TYPE),
	NodeType(NodeType::NODE_TYPEDEF),
	NodeType(NodeType::NODE_ENUM)
};

int NodeType::typeToInt(NodeType::Type type)
{
	return type;
}

NodeType::Type NodeType::intToType(int value)
{
	switch (value)
	{
	case NodeType::NODE_TYPE:
		return NodeType::NODE_TYPE;
	case NodeType::NODE_BUILTIN_TYPE:
		return NodeType::NODE_BUILTIN_TYPE;
	case NodeType::NODE_MODULE:
		return NodeType::NODE_MODULE;
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
	case NodeType::NODE_ANNOTATION:
		return NodeType::NODE_ANNOTATION;
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

std::string NodeType::getReadableTypeString(NodeType::Type type)
{
	switch (type)
	{
	case NodeType::NODE_SYMBOL:
		return "symbol";
	case NodeType::NODE_BUILTIN_TYPE:
		return "built-in type";
	case NodeType::NODE_TYPE:
		return "type";
	case NodeType::NODE_MODULE:
		return "module";
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
	case NodeType::NODE_ANNOTATION:
		return "annotation";
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

std::wstring NodeType::getReadableTypeWString(NodeType::Type type)
{
	std::string str = getReadableTypeString(type);
	return std::wstring(str.begin(), str.end());
}

NodeType::Type NodeType::getTypeForReadableTypeString(const std::wstring& str)
{
	for (NodeType::TypeMask mask = 1; mask <= NodeType::NODE_MAX_VALUE; mask *= 2)
	{
		NodeType::Type type = intToType(mask);
		if (getReadableTypeWString(type) == str)
		{
			return type;
		}
	}

	return NodeType::NODE_SYMBOL;
}

NodeType::NodeType(Type type): m_type(type) {}

bool NodeType::operator==(const NodeType& o) const
{
	return m_type == o.m_type;
}

bool NodeType::operator!=(const NodeType& o) const
{
	return !operator==(o);
}

bool NodeType::operator<(const NodeType& o) const
{
	return m_type < o.m_type;
}

NodeType::Type NodeType::getType() const
{
	return m_type;
}

Id NodeType::getId() const
{
	// TODO: add id in constructor and return it here
	return typeToInt(m_type);
}

bool NodeType::isFile() const
{
	const NodeType::TypeMask mask = NodeType::NODE_FILE;
	return ((m_type & mask) > 0);
}

bool NodeType::isBuiltin() const
{
	const NodeType::TypeMask mask = NodeType::NODE_BUILTIN_TYPE;
	return ((m_type & mask) > 0);
}

bool NodeType::isUnknownSymbol() const
{
	const NodeType::TypeMask mask = NodeType::NODE_SYMBOL;
	return ((m_type & mask) > 0);
}

bool NodeType::isInheritable() const
{
	// what about java enums?
	const NodeType::TypeMask mask = NodeType::NODE_SYMBOL | NodeType::NODE_BUILTIN_TYPE |
		NodeType::NODE_TYPE | NodeType::NODE_STRUCT | NodeType::NODE_CLASS |
		NodeType::NODE_INTERFACE;

	return ((m_type & mask) > 0);
}

bool NodeType::isPackage() const
{
	const NodeType::TypeMask mask = NodeType::NODE_MODULE | NodeType::NODE_NAMESPACE |
		NodeType::NODE_PACKAGE;
	return ((m_type & mask) > 0);
}

bool NodeType::isCallable() const
{
	const NodeType::TypeMask mask = NodeType::NODE_FUNCTION | NodeType::NODE_METHOD;
	return ((m_type & mask) > 0);
}

bool NodeType::isVariable() const
{
	const NodeType::TypeMask mask = NodeType::NODE_GLOBAL_VARIABLE | NodeType::NODE_FIELD;
	return ((m_type & mask) > 0);
}

bool NodeType::isUsable() const
{
	const NodeType::TypeMask mask = NodeType::NODE_SYMBOL | NodeType::NODE_BUILTIN_TYPE |
		NodeType::NODE_STRUCT | NodeType::NODE_CLASS | NodeType::NODE_ENUM | NodeType::NODE_UNION |
		NodeType::NODE_INTERFACE | NodeType::NODE_ANNOTATION | NodeType::NODE_TYPEDEF;
	return ((m_type & mask) > 0);
}

bool NodeType::isPotentialMember() const
{
	const NodeType::TypeMask mask = NodeType::NODE_METHOD | NodeType::NODE_FIELD |
		NodeType::NODE_CLASS | NodeType::NODE_INTERFACE | NodeType::NODE_ANNOTATION |
		NodeType::NODE_STRUCT | NodeType::NODE_UNION | NodeType::NODE_TYPEDEF | NodeType::NODE_ENUM;

	return ((m_type & mask) > 0);
}

bool NodeType::isCollapsible() const
{
	const NodeType::TypeMask mask = NodeType::NODE_SYMBOL | NodeType::NODE_TYPE |
		NodeType::NODE_BUILTIN_TYPE | NodeType::NODE_STRUCT | NodeType::NODE_CLASS |
		NodeType::NODE_INTERFACE | NodeType::NODE_ANNOTATION | NodeType::NODE_ENUM |
		NodeType::NODE_UNION | NodeType::NODE_FILE;
	return ((m_type & mask) > 0);
}

bool NodeType::isVisibleAsParentInGraph() const
{
	return !isPackage();
}

bool NodeType::hasSearchFilter() const
{
	const NodeType::TypeMask mask = NodeType::NODE_BUILTIN_TYPE | NodeType::NODE_MODULE |
		NodeType::NODE_NAMESPACE | NodeType::NODE_PACKAGE | NodeType::NODE_STRUCT |
		NodeType::NODE_CLASS | NodeType::NODE_INTERFACE | NodeType::NODE_ANNOTATION |
		NodeType::NODE_GLOBAL_VARIABLE | NodeType::NODE_FIELD | NodeType::NODE_FUNCTION |
		NodeType::NODE_METHOD | NodeType::NODE_ENUM | NodeType::NODE_ENUM_CONSTANT |
		NodeType::NODE_TYPEDEF | NodeType::NODE_FILE | NodeType::NODE_MACRO | NodeType::NODE_UNION;
	return ((m_type & mask) > 0);
}

Tree<NodeType::BundleInfo> NodeType::getOverviewBundleTree() const
{
	switch (m_type)
	{
	case NodeType::NODE_FILE:
		return Tree<BundleInfo>(BundleInfo(L"Files"));
	case NodeType::NODE_MACRO:
		return Tree<BundleInfo>(BundleInfo(L"Macros"));
	case NodeType::NODE_NAMESPACE:
	{
		Tree<BundleInfo> tree(BundleInfo(L"Namespaces"));
		tree.children.push_back(Tree<BundleInfo>(BundleInfo(
			[](const std::wstring& nodeName) {
				return nodeName.find(L"anonymous namespace") != std::wstring::npos;
			},
			L"Anonymous Namespaces")));
		return tree;
	}
	case NodeType::NODE_MODULE:
		return Tree<BundleInfo>(BundleInfo(L"Modules"));
	case NodeType::NODE_PACKAGE:
		return Tree<BundleInfo>(BundleInfo(L"Packages"));
	case NodeType::NODE_CLASS:
		return Tree<BundleInfo>(BundleInfo(L"Classes"));
	case NodeType::NODE_INTERFACE:
		return Tree<BundleInfo>(BundleInfo(L"Interfaces"));
	case NodeType::NODE_ANNOTATION:
		return Tree<BundleInfo>(BundleInfo(L"Annotations"));
	case NodeType::NODE_STRUCT:
		return Tree<BundleInfo>(BundleInfo(L"Structs"));
	case NodeType::NODE_FUNCTION:
		return Tree<BundleInfo>(BundleInfo(L"Functions"));
	case NodeType::NODE_GLOBAL_VARIABLE:
		return Tree<BundleInfo>(BundleInfo(L"Global Variables"));
	case NodeType::NODE_TYPE:
		return Tree<BundleInfo>(BundleInfo(L"Types"));
	case NodeType::NODE_TYPEDEF:
		return Tree<BundleInfo>(BundleInfo(L"Typedefs"));
	case NodeType::NODE_ENUM:
		return Tree<BundleInfo>(BundleInfo(L"Enums"));
	case NodeType::NODE_UNION:
		return Tree<BundleInfo>(BundleInfo(L"Unions"));
	default:
		break;
	}

	return Tree<BundleInfo>();
}

FilePath NodeType::getIconPath() const
{
	if (isPackage())
	{
		// this icon cannot be changed
		return ResourcePaths::getGuiPath().concatenate(L"graph_view/images/namespace.png");
	}

	switch (m_type)
	{
	case NodeType::NODE_ANNOTATION:
		return ResourcePaths::getGuiPath().concatenate(L"graph_view/images/annotation.png");
	case NodeType::NODE_ENUM:
		return ResourcePaths::getGuiPath().concatenate(L"graph_view/images/enum.png");
	case NodeType::NODE_TYPEDEF:
		return ResourcePaths::getGuiPath().concatenate(L"graph_view/images/typedef.png");
	case NodeType::NODE_MACRO:
		return ResourcePaths::getGuiPath().concatenate(L"graph_view/images/macro.png");
	case NodeType::NODE_FILE:
		return ResourcePaths::getGuiPath().concatenate(L"graph_view/images/file.png");
	default:
		return FilePath();
	}
}

bool NodeType::hasIcon() const
{
	if (isPackage())
	{
		return true;
	}

	const NodeType::TypeMask mask = NODE_ANNOTATION | NodeType::NODE_ENUM | NodeType::NODE_TYPEDEF |
		NodeType::NODE_FILE | NodeType::NODE_MACRO;
	return ((m_type & mask) > 0);
}

NodeType::StyleType NodeType::getNodeStyle() const
{
	switch (m_type)
	{
	case NodeType::NODE_MODULE:
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
	case NodeType::NODE_ANNOTATION:
	case NodeType::NODE_ENUM:
	case NodeType::NODE_TYPEDEF:
	case NodeType::NODE_TYPE_PARAMETER:
	case NodeType::NODE_FILE:
	case NodeType::NODE_MACRO:
		return STYLE_BIG_NODE;
	case NodeType::NODE_FUNCTION:
	case NodeType::NODE_METHOD:
	case NodeType::NODE_GLOBAL_VARIABLE:
	case NodeType::NODE_FIELD:
	case NodeType::NODE_ENUM_CONSTANT:
	default:
		return STYLE_SMALL_NODE;
	}
}

bool NodeType::hasOverviewBundle() const
{
	return !getOverviewBundleTree().data.isValid();
}

std::string NodeType::getUnderscoredTypeString() const
{
	return utility::replace(utility::replace(getReadableTypeString(), "-", "_"), " ", "_");
}

std::string NodeType::getReadableTypeString() const
{
	return getReadableTypeString(m_type);
}

std::wstring NodeType::getUnderscoredTypeWString() const
{
	std::string str = getUnderscoredTypeString();
	return std::wstring(str.begin(), str.end());
}

std::wstring NodeType::getReadableTypeWString() const
{
	std::string str = getReadableTypeString();
	return std::wstring(str.begin(), str.end());
}
