#ifndef DUMMY_NODE_H
#define DUMMY_NODE_H

#include "utility/math/Vector2.h"
#include "utility/types.h"
#include "utility/utility.h"
#include "utility/utilityString.h"

#include "data/graph/Node.h"
#include "data/name/NameHierarchy.h"
#include "data/parser/AccessKind.h"

// temporary data structure for (visual) graph creation process
struct DummyNode
{
public:
	struct DummyNodeComp
	{
		bool operator()(const std::shared_ptr<DummyNode> a, const std::shared_ptr<DummyNode> b) const
		{
			if (a->bundleId != b->bundleId)
			{
				return a->bundleId > b->bundleId;
			}
			else if (a->isBundleNode() != b->isBundleNode())
			{
				return a->isBundleNode();
			}

			return utility::toLowerCase(a->name) < utility::toLowerCase(b->name);
		}
	};

	typedef std::multiset<std::shared_ptr<DummyNode>, DummyNodeComp> BundledNodesSet;

	struct BundleInfo
	{
		BundleInfo()
			: isActive(false)
			, isDefined(false)
			, layoutVertical(false)
			, isReferenced(false)
			, isReferencing(false)
		{}

		bool isActive;
		bool isDefined;
		bool layoutVertical;
		bool isReferenced;
		bool isReferencing;
	};

	DummyNode()
		: visible(false)
		, childVisible(false)
		, tokenId(0)
		, data(nullptr)
		, active(false)
		, connected(false)
		, expanded(false)
		, autoExpanded(false)
		, hasParent(true)
		, hasQualifier(false)
		, accessKind(ACCESS_NONE)
		, isAccess(false)
		, invisibleSubNodeCount(0)
		, bundleId(0)
		, layoutBucket(0, 0)
		, bundledNodeCount(0)
		, bundledNodeType(Node::NODE_NON_INDEXED)
		, qualifierName(NAME_DELIMITER_UNKNOWN)
		, textNode(false)
	{
	}

	bool isGraphNode() const
	{
		return data != nullptr;
	}

	bool isAccessNode() const
	{
		return isAccess;
	}

	bool isExpandToggleNode() const
	{
		return !isGraphNode() && !isAccessNode() && !isBundleNode() && !isQualifierNode() && !isTextNode();
	}

	bool isBundleNode() const
	{
		return bundledNodes.size() > 0;
	}

	bool isQualifierNode() const
	{
		return qualifierName.size();
	}

	bool isTextNode() const
	{
		return textNode;
	}

	bool isExpanded() const
	{
		return expanded;
	}

	bool hasVisibleSubNode() const
	{
		for (std::shared_ptr<DummyNode> node : subNodes)
		{
			if (node->visible)
			{
				return true;
			}
		}

		return false;
	}

	bool hasActiveSubNode() const
	{
		if (active)
		{
			return true;
		}

		for (std::shared_ptr<DummyNode> node : subNodes)
		{
			if (node->hasActiveSubNode())
			{
				return true;
			}
		}

		return false;
	}

	size_t getActiveSubNodeCount() const
	{
		size_t count = 0;

		if (active)
		{
			count += 1;
		}

		for (std::shared_ptr<DummyNode> node : subNodes)
		{
			count += node->getActiveSubNodeCount();
		}

		return count;
	}

	bool hasConnectedSubNode() const
	{
		if (connected)
		{
			return true;
		}

		for (std::shared_ptr<DummyNode> node : subNodes)
		{
			if (node->hasConnectedSubNode())
			{
				return true;
			}
		}

		return false;
	}

	std::vector<const DummyNode*> getConnectedSubNodes() const
	{
		std::vector<const DummyNode*> nodes;

		if (connected)
		{
			nodes.push_back(this);
		}

		for (std::shared_ptr<DummyNode> node : subNodes)
		{
			utility::append(nodes, node->getConnectedSubNodes());
		}

		return nodes;
	}

	std::vector<const DummyNode*> getAllBundledNodes() const
	{
		std::vector<const DummyNode*> nodes;
		for (std::shared_ptr<DummyNode> node : bundledNodes)
		{
			utility::append(nodes, node->getConnectedSubNodes());
		}
		return nodes;
	}

	size_t getBundledNodeCount() const
	{
		if (bundledNodeCount > 0)
		{
			return bundledNodeCount;
		}

		return bundledNodes.size();
	}

	void forEachDummyNodeRecursive(std::function<void(DummyNode*)> func)
	{
		func(this);

		for (std::shared_ptr<DummyNode> node : subNodes)
		{
			node->forEachDummyNodeRecursive(func);
		}
	}

	Id setBundleIdRecursive(Id bundleId)
	{
		if (isBundleNode())
		{
			bundleId++;
		}

		this->bundleId = bundleId;

		for (std::shared_ptr<DummyNode> node : bundledNodes)
		{
			bundleId = node->setBundleIdRecursive(bundleId);
		}

		return bundleId;
	}

	Vec2i position;
	Vec2i size;

	bool visible;
	bool childVisible;

	Id tokenId;

	std::vector<std::shared_ptr<DummyNode>> subNodes;

	// GraphNode
	const Node* data;
	std::string name;

	bool active;
	bool connected;
	bool expanded;
	bool autoExpanded;
	bool hasParent;
	bool hasQualifier;

	// AccessNode
	AccessKind accessKind;
	bool isAccess;

	// ExpandToggleNode
	size_t invisibleSubNodeCount;

	// Bundling
	BundleInfo bundleInfo;
	Id bundleId;

	// Layout
	Vec2i layoutBucket;

	// BundleNode
	BundledNodesSet bundledNodes;
	size_t bundledNodeCount;
	Node::NodeType bundledNodeType;

	// QualifierNode
	NameHierarchy qualifierName;

	// TextNode
	bool textNode;
};

#endif // DUMMY_NODE_H
