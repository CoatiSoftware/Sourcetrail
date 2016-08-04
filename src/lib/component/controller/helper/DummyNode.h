#ifndef DUMMY_NODE_H
#define DUMMY_NODE_H

#include "utility/math/Vector2.h"
#include "utility/types.h"
#include "utility/utility.h"

#include "data/graph/token_component/TokenComponentAccess.h"

class Node;

// temporary data structure for (visual) graph creation process
struct DummyNode
{
public:
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
		, topLevelAncestorId(0)
		, tokenId(0)
		, data(nullptr)
		, active(false)
		, connected(false)
		, expanded(false)
		, hasParent(true)
		, accessKind(ACCESS_NONE)
		, invisibleSubNodeCount(0)
		, layoutBucket(0, 0)
		, bundledNodeCount(0)
	{
	}

	bool isGraphNode() const
	{
		return data != nullptr;
	}

	bool isAccessNode() const
	{
		return accessKind != ACCESS_NONE;
	}

	bool isExpandToggleNode() const
	{
		return !isGraphNode() && !isAccessNode() && !isBundleNode();
	}

	bool isBundleNode() const
	{
		return bundledNodes.size() > 0;
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

	size_t getConnectedSubNodeCount() const
	{
		size_t count = 0;

		if (connected)
		{
			count += 1;
		}

		for (std::shared_ptr<DummyNode> node : subNodes)
		{
			count += node->getConnectedSubNodeCount();
		}

		return count;
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

	Vec2i position;
	Vec2i size;

	bool visible;
	bool childVisible;

	Id topLevelAncestorId;
	Id tokenId;

	std::vector<std::shared_ptr<DummyNode>> subNodes;

	// GraphNode
	const Node* data;
	std::string name;

	bool active;
	bool connected;
	bool expanded;
	bool hasParent;

	// AccessNode
	AccessKind accessKind;

	// ExpandToggleNode
	size_t invisibleSubNodeCount;

	// Bundling
	BundleInfo bundleInfo;

	// Layout
	Vec2i layoutBucket;

	// BundleNode
	std::vector<std::shared_ptr<DummyNode>> bundledNodes;
	size_t bundledNodeCount;
};

#endif // DUMMY_NODE_H
