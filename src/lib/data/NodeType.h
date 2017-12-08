#ifndef NODE_TYPE_H
#define NODE_TYPE_H

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "utility/file/FilePath.h"
#include "utility/Tree.h"
#include "utility/types.h"

class NodeType
{
public:
	typedef int TypeMask;
	enum Type : NodeType::TypeMask
	{ // make sure that the value of 0x0 is not used here because it doesn't work for bitmasking.
		NODE_SYMBOL = 1 << 0,
		NODE_TYPE = 1 << 1,
		NODE_BUILTIN_TYPE = 1 << 2,

		NODE_NAMESPACE = 1 << 3,
		NODE_PACKAGE = 1 << 4,
		NODE_STRUCT = 1 << 5,
		NODE_CLASS = 1 << 6,
		NODE_INTERFACE = 1 << 7,
		NODE_GLOBAL_VARIABLE = 1 << 8,
		NODE_FIELD = 1 << 9,
		NODE_FUNCTION = 1 << 10,
		NODE_METHOD = 1 << 11,

		NODE_ENUM = 1 << 12,
		NODE_ENUM_CONSTANT = 1 << 13,
		NODE_TYPEDEF = 1 << 14,
		NODE_TEMPLATE_PARAMETER_TYPE = 1 << 15,
		NODE_TYPE_PARAMETER = 1 << 16,

		NODE_FILE = 1 << 17,
		NODE_MACRO = 1 << 18,
		NODE_UNION = 1 << 19,

		NODE_MAX_VALUE = NODE_UNION
	};

	enum StyleType
	{
		STYLE_PACKAGE = 0,
		STYLE_SMALL_NODE = 1,
		STYLE_BIG_NODE = 2
	};

	struct BundleInfo
	{
		BundleInfo()
			: nameMatcher(nullptr)
			, bundleName("")
		{}
		BundleInfo(std::function<bool(std::string)> nameMatcher, std::string bundleName)
			: nameMatcher(nameMatcher)
			, bundleName(bundleName)
		{}
		std::function<bool(const std::string&)> nameMatcher;
		std::string bundleName;
	};

	NodeType(Type type);

	bool operator==(const NodeType& o) const;
	bool operator<(const NodeType& o) const;

	Type getType() const;

	Id getId() const;
	bool isFile() const;
	bool isBuiltin() const;
	bool isUnknownSymbol() const;
	bool isInheritable() const;
	bool isPackage() const;
	bool isCallable() const;
	bool isVariable() const;
	bool isUsable() const;
	bool isPotentialMember() const;
	bool isCollapsible() const;
	bool isVisibleAsParentInGraph() const;
	Tree<BundleInfo> getOverviewBundleTree() const;

	FilePath getIconPath() const;

	bool hasIcon() const;
	StyleType getNodeStyle() const;

	bool hasOverviewBundle() const;
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
