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
		, aggregated(false)
		, expanded(false)
		, autoExpanded(false)
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
		return !data && !isAccessNode();
	}

	bool isExpanded() const
	{
		return expanded || autoExpanded;
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
	bool active;
	bool connected;
	bool aggregated;
	bool expanded;
	bool autoExpanded;

	// AccessNode
	TokenComponentAccess::AccessType accessType;

	// ExpandToggleNode
	size_t invisibleSubNodeCount;
};

#endif // DUMMY_NODE_H
