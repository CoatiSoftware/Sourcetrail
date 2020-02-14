#include "NodeType.h"

#include "ResourcePaths.h"
#include "utilityString.h"

std::vector<NodeType> const NodeType::overviewBundleNodeTypesOrdered = {
	NodeType(NODE_FILE),
	NodeType(NODE_MACRO),
	NodeType(NODE_ANNOTATION),
	NodeType(NODE_MODULE),
	NodeType(NODE_NAMESPACE),
	NodeType(NODE_PACKAGE),
	NodeType(NODE_CLASS),
	NodeType(NODE_INTERFACE),
	NodeType(NODE_STRUCT),
	NodeType(NODE_UNION),
	NodeType(NODE_FUNCTION),
	NodeType(NODE_GLOBAL_VARIABLE),
	NodeType(NODE_TYPE),
	NodeType(NODE_TYPEDEF),
	NodeType(NODE_ENUM)};


NodeType::NodeType(NodeKind kind): m_kind(kind) {}

bool NodeType::operator==(const NodeType& o) const
{
	return m_kind == o.m_kind;
}

bool NodeType::operator!=(const NodeType& o) const
{
	return !operator==(o);
}

bool NodeType::operator<(const NodeType& o) const
{
	return m_kind < o.m_kind;
}
NodeKind NodeType::getKind() const
{
	return m_kind;
}

Id NodeType::getId() const
{
	// TODO: add id in constructor and return it here
	return nodeKindToInt(m_kind);
}

bool NodeType::isFile() const
{
	const NodeKindMask mask = NODE_FILE;
	return ((m_kind & mask) > 0);
}

bool NodeType::isBuiltin() const
{
	const NodeKindMask mask = NODE_BUILTIN_TYPE;
	return ((m_kind & mask) > 0);
}

bool NodeType::isUnknownSymbol() const
{
	const NodeKindMask mask = NODE_SYMBOL;
	return ((m_kind & mask) > 0);
}

bool NodeType::isInheritable() const
{
	// what about java enums?
	const NodeKindMask mask = NODE_SYMBOL | NODE_BUILTIN_TYPE | NODE_TYPE | NODE_STRUCT |
		NODE_CLASS | NODE_INTERFACE;

	return ((m_kind & mask) > 0);
}

bool NodeType::isPackage() const
{
	const NodeKindMask mask = NODE_MODULE | NODE_NAMESPACE | NODE_PACKAGE;
	return ((m_kind & mask) > 0);
}

bool NodeType::isCallable() const
{
	const NodeKindMask mask = NODE_FUNCTION | NODE_METHOD;
	return ((m_kind & mask) > 0);
}

bool NodeType::isVariable() const
{
	const NodeKindMask mask = NODE_GLOBAL_VARIABLE | NODE_FIELD;
	return ((m_kind & mask) > 0);
}

bool NodeType::isUsable() const
{
	const NodeKindMask mask = NODE_SYMBOL | NODE_BUILTIN_TYPE | NODE_STRUCT | NODE_CLASS |
		NODE_ENUM | NODE_UNION | NODE_INTERFACE | NODE_ANNOTATION | NODE_TYPEDEF;
	return ((m_kind & mask) > 0);
}

bool NodeType::isPotentialMember() const
{
	const NodeKindMask mask = NODE_METHOD | NODE_FIELD | NODE_CLASS | NODE_INTERFACE |
		NODE_ANNOTATION | NODE_STRUCT | NODE_UNION | NODE_TYPEDEF | NODE_ENUM;

	return ((m_kind & mask) > 0);
}

bool NodeType::isCollapsible() const
{
	const NodeKindMask mask = NODE_SYMBOL | NODE_TYPE | NODE_BUILTIN_TYPE | NODE_STRUCT |
		NODE_CLASS | NODE_INTERFACE | NODE_ANNOTATION | NODE_ENUM | NODE_UNION | NODE_FILE;
	return ((m_kind & mask) > 0);
}

bool NodeType::isVisibleAsParentInGraph() const
{
	return !isPackage();
}

bool NodeType::hasSearchFilter() const
{
	const NodeKindMask mask = NODE_BUILTIN_TYPE | NODE_MODULE | NODE_NAMESPACE | NODE_PACKAGE |
		NODE_STRUCT | NODE_CLASS | NODE_INTERFACE | NODE_ANNOTATION | NODE_GLOBAL_VARIABLE |
		NODE_FIELD | NODE_FUNCTION | NODE_METHOD | NODE_ENUM | NODE_ENUM_CONSTANT | NODE_TYPEDEF |
		NODE_FILE | NODE_MACRO | NODE_UNION;
	return ((m_kind & mask) > 0);
}

Tree<NodeType::BundleInfo> NodeType::getOverviewBundleTree() const
{
	switch (m_kind)
	{
	case NODE_FILE:
		return Tree<BundleInfo>(BundleInfo(L"Files"));
	case NODE_MACRO:
		return Tree<BundleInfo>(BundleInfo(L"Macros"));
	case NODE_NAMESPACE:
	{
		Tree<BundleInfo> tree(BundleInfo(L"Namespaces"));
		tree.children.push_back(Tree<BundleInfo>(BundleInfo(
			[](const std::wstring& nodeName) {
				return nodeName.find(L"anonymous namespace") != std::wstring::npos;
			},
			L"Anonymous Namespaces")));
		return tree;
	}
	case NODE_MODULE:
		return Tree<BundleInfo>(BundleInfo(L"Modules"));
	case NODE_PACKAGE:
		return Tree<BundleInfo>(BundleInfo(L"Packages"));
	case NODE_CLASS:
		return Tree<BundleInfo>(BundleInfo(L"Classes"));
	case NODE_INTERFACE:
		return Tree<BundleInfo>(BundleInfo(L"Interfaces"));
	case NODE_ANNOTATION:
		return Tree<BundleInfo>(BundleInfo(L"Annotations"));
	case NODE_STRUCT:
		return Tree<BundleInfo>(BundleInfo(L"Structs"));
	case NODE_FUNCTION:
		return Tree<BundleInfo>(BundleInfo(L"Functions"));
	case NODE_GLOBAL_VARIABLE:
		return Tree<BundleInfo>(BundleInfo(L"Global Variables"));
	case NODE_TYPE:
		return Tree<BundleInfo>(BundleInfo(L"Types"));
	case NODE_TYPEDEF:
		return Tree<BundleInfo>(BundleInfo(L"Typedefs"));
	case NODE_ENUM:
		return Tree<BundleInfo>(BundleInfo(L"Enums"));
	case NODE_UNION:
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

	switch (m_kind)
	{
	case NODE_ANNOTATION:
		return ResourcePaths::getGuiPath().concatenate(L"graph_view/images/annotation.png");
	case NODE_ENUM:
		return ResourcePaths::getGuiPath().concatenate(L"graph_view/images/enum.png");
	case NODE_TYPEDEF:
		return ResourcePaths::getGuiPath().concatenate(L"graph_view/images/typedef.png");
	case NODE_MACRO:
		return ResourcePaths::getGuiPath().concatenate(L"graph_view/images/macro.png");
	case NODE_FILE:
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

	const NodeKindMask mask = NODE_ANNOTATION | NODE_ENUM | NODE_TYPEDEF | NODE_FILE | NODE_MACRO;
	return ((m_kind & mask) > 0);
}

NodeType::StyleType NodeType::getNodeStyle() const
{
	switch (m_kind)
	{
	case NODE_MODULE:
	case NODE_NAMESPACE:
	case NODE_PACKAGE:
		return STYLE_PACKAGE;
	case NODE_SYMBOL:
	case NODE_TYPE:
	case NODE_BUILTIN_TYPE:
	case NODE_STRUCT:
	case NODE_CLASS:
	case NODE_UNION:
	case NODE_INTERFACE:
	case NODE_ANNOTATION:
	case NODE_ENUM:
	case NODE_TYPEDEF:
	case NODE_TYPE_PARAMETER:
	case NODE_FILE:
	case NODE_MACRO:
		return STYLE_BIG_NODE;
	case NODE_FUNCTION:
	case NODE_METHOD:
	case NODE_GLOBAL_VARIABLE:
	case NODE_FIELD:
	case NODE_ENUM_CONSTANT:
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
	return getReadableNodeKindString(m_kind);
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
