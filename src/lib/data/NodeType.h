#ifndef NODE_TYPE_H
#define NODE_TYPE_H

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <vector>

#include "FilePath.h"
#include "Tree.h"
#include "types.h"

class NodeType
{
public:
	typedef int TypeMask;
	enum Type : NodeType::TypeMask
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

	enum StyleType
	{
		STYLE_PACKAGE = 0,
		STYLE_SMALL_NODE = 1,
		STYLE_BIG_NODE = 2,
		STYLE_GROUP = 3
	};

	struct BundleInfo
	{
		BundleInfo() {}

		BundleInfo(std::wstring bundleName)
			: nameMatcher([](const std::wstring&) { return true; }), bundleName(bundleName)
		{
		}

		BundleInfo(std::function<bool(std::wstring)> nameMatcher, std::wstring bundleName)
			: nameMatcher(nameMatcher), bundleName(bundleName)
		{
		}

		bool isValid() const
		{
			return bundleName.size() > 0;
		}

		std::function<bool(const std::wstring&)> nameMatcher = nullptr;
		std::wstring bundleName;
	};

	static std::vector<NodeType> getOverviewBundleNodeTypesOrdered();

	static int typeToInt(NodeType::Type type);
	static NodeType::Type intToType(int value);

	static std::string getReadableTypeString(NodeType::Type type);
	static std::wstring getReadableTypeWString(NodeType::Type type);
	static NodeType::Type getTypeForReadableTypeString(const std::wstring& str);

	NodeType(Type type);

	bool operator==(const NodeType& o) const;
	bool operator!=(const NodeType& o) const;
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
	bool hasSearchFilter() const;
	Tree<BundleInfo> getOverviewBundleTree() const;

	FilePath getIconPath() const;

	bool hasIcon() const;
	StyleType getNodeStyle() const;

	bool hasOverviewBundle() const;
	std::string getUnderscoredTypeString() const;
	std::string getReadableTypeString() const;
	std::wstring getUnderscoredTypeWString() const;
	std::wstring getReadableTypeWString() const;

	static std::vector<NodeType> const overviewBundleNodeTypesOrdered;
private:
	Type m_type;
};

#endif	  // NODE_TYPE_H
