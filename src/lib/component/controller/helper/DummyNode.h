#ifndef DUMMY_NODE_H
#define DUMMY_NODE_H

#include "utility/math/Vector2.h"
#include "utility/types.h"

#include "data/graph/token_component/TokenComponentAccess.h"

class Node;

// temporary data structure for (visual) graph creation process
struct DummyNode
{
public:
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
		, accessType(TokenComponentAccess::ACCESS_NONE)
		, invisibleSubNodeCount(0)
	{
	}

	bool isGraphNode() const
	{
		return data != nullptr;
	}

	bool isAccessNode() const
	{
		return accessType != TokenComponentAccess::ACCESS_NONE;
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
		for (const DummyNode& node : subNodes)
		{
			if (node.visible)
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

		for (const DummyNode& node : subNodes)
		{
			if (node.hasActiveSubNode())
			{
				return true;
			}
		}

		return false;
	}

	bool hasConnectedSubNode() const
	{
		if (connected)
		{
			return true;
		}

		for (const DummyNode& node : subNodes)
		{
			if (node.hasConnectedSubNode())
			{
				return true;
			}
		}

		return false;
	}

	Vec2i position;
	Vec2i size;

	bool visible;
	bool childVisible;

	Id topLevelAncestorId;
	Id tokenId;

	std::vector<DummyNode> subNodes;

	// GraphNode
	const Node* data;
	std::string name;

	bool active;
	bool connected;
	bool expanded;
	bool hasParent;

	// AccessNode
	TokenComponentAccess::AccessType accessType;

	// ExpandToggleNode
	size_t invisibleSubNodeCount;

	// BundleNode
	std::vector<DummyNode> bundledNodes;
};

#endif // DUMMY_NODE_H
