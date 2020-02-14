#ifndef DUMMY_NODE_H
#define DUMMY_NODE_H

#include "Vector2.h"
#include "Vector4.h"
#include "types.h"
#include "utility.h"
#include "utilityString.h"

#include "AccessKind.h"
#include "GroupType.h"
#include "NameHierarchy.h"
#include "Node.h"

// temporary data structure for (visual) graph creation process
struct DummyNode
{
public:
	enum Type
	{
		DUMMY_DATA,
		DUMMY_ACCESS,
		DUMMY_EXPAND_TOGGLE,
		DUMMY_BUNDLE,
		DUMMY_QUALIFIER,
		DUMMY_TEXT,
		DUMMY_GROUP
	};

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

			return utility::caseInsensitiveLess(a->name, b->name);
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
		{
		}

		static BundleInfo averageBundleInfo(const std::vector<DummyNode::BundleInfo>& bundleInfos)
		{
			size_t activeCount = 0;
			size_t definedCount = 0;
			size_t verticalLayoutCount = 0;
			size_t referencedCount = 0;
			size_t referencingCount = 0;

			for (const BundleInfo& info: bundleInfos)
			{
				if (info.isActive)
					activeCount++;
				if (info.isDefined)
					definedCount++;
				if (info.layoutVertical)
					verticalLayoutCount++;
				if (info.isReferenced)
					referencedCount++;
				if (info.isReferencing)
					referencingCount++;
			}

			BundleInfo info;
			if (activeCount >= std::ceil(bundleInfos.size() / 2.0f))
				info.isActive = true;
			if (definedCount >= std::ceil(bundleInfos.size() / 2.0f))
				info.isDefined = true;
			if (verticalLayoutCount >= std::ceil(bundleInfos.size() / 2.0f))
				info.layoutVertical = true;
			if (referencedCount >= std::ceil(bundleInfos.size() / 2.0f))
				info.isReferenced = true;
			if (referencingCount >= std::ceil(bundleInfos.size() / 2.0f))
				info.isReferencing = true;
			return info;
		}

		bool isActive;
		bool isDefined;
		bool layoutVertical;
		bool isReferenced;
		bool isReferencing;
	};

	DummyNode(Type type)
		: type(type)
		, visible(false)
		, hidden(false)
		, childVisible(false)
		, tokenId(0)
		, data(nullptr)
		, active(false)
		, connected(false)
		, expanded(false)
		, autoExpanded(false)
		, hasParent(true)
		, accessKind(ACCESS_NONE)
		, invisibleSubNodeCount(0)
		, bundleId(0)
		, bundledNodeCount(0)
		, bundledNodeType(NODE_SYMBOL)
		, qualifierName(NAME_DELIMITER_UNKNOWN)
		, groupType(GroupType::DEFAULT)
		, groupLayout(GroupLayout::LIST)
		, interactive(true)
		, fontSizeDiff(5)
	{
	}

	bool isGraphNode() const
	{
		return type == DUMMY_DATA;
	}

	bool isAccessNode() const
	{
		return type == DUMMY_ACCESS;
	}

	bool isExpandToggleNode() const
	{
		return type == DUMMY_EXPAND_TOGGLE;
	}

	bool isBundleNode() const
	{
		return type == DUMMY_BUNDLE;
	}

	bool isQualifierNode() const
	{
		return type == DUMMY_QUALIFIER;
	}

	bool isTextNode() const
	{
		return type == DUMMY_TEXT;
	}

	bool isGroupNode() const
	{
		return type == DUMMY_GROUP;
	}

	bool isExpanded() const
	{
		return expanded;
	}

	bool hasVisibleSubNode() const
	{
		for (const std::shared_ptr<DummyNode>& node: subNodes)
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

		for (const std::shared_ptr<DummyNode>& node: subNodes)
		{
			if (node->hasActiveSubNode())
			{
				return true;
			}
		}

		return false;
	}

	Vec4i getActiveSubNodeRect(Vec2i pos = Vec2i()) const
	{
		pos += position;

		if (active)
		{
			return Vec4i(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
		}

		for (const std::shared_ptr<DummyNode>& node: subNodes)
		{
			Vec4i rect = node->getActiveSubNodeRect(pos);
			if (rect.w() > 0)
			{
				return rect;
			}
		}

		return Vec4i();
	}

	size_t getActiveSubNodeCount() const
	{
		size_t count = 0;

		if (active)
		{
			count += 1;
		}

		for (const std::shared_ptr<DummyNode>& node: subNodes)
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

		for (const std::shared_ptr<DummyNode>& node: subNodes)
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

		for (const std::shared_ptr<DummyNode>& node: subNodes)
		{
			utility::append(nodes, node->getConnectedSubNodes());
		}

		return nodes;
	}

	std::vector<const DummyNode*> getAllBundledNodes() const
	{
		std::vector<const DummyNode*> nodes;
		for (const std::shared_ptr<DummyNode>& node: bundledNodes)
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

		for (const std::shared_ptr<DummyNode>& node: subNodes)
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

		for (const std::shared_ptr<DummyNode>& node: bundledNodes)
		{
			bundleId = node->setBundleIdRecursive(bundleId);
		}

		return bundleId;
	}

	bool hasMissingChildNodes() const
	{
		size_t childCount = 0;
		bool implicit = false;
		if (isGraphNode())
		{
			childCount = data->getChildCount();
			implicit = data->isImplicit();
		}

		size_t subNodeCount = 0;
		for (const std::shared_ptr<DummyNode>& subNode: subNodes)
		{
			if (subNode->isAccessNode())
			{
				for (const std::shared_ptr<DummyNode>& subSubNode: subNode->subNodes)
				{
					if (subSubNode->isGraphNode() && (implicit || !subSubNode->data->isImplicit()))
					{
						subNodeCount++;
					}
				}
			}
		}

		return subNodeCount < childCount;
	}

	std::map<Id, std::shared_ptr<DummyNode>> getSubGraphNodes() const
	{
		std::map<Id, std::shared_ptr<DummyNode>> subGraphNodes;

		for (const std::shared_ptr<DummyNode>& subNode: subNodes)
		{
			if (subNode->isAccessNode())
			{
				for (const std::shared_ptr<DummyNode>& subSubNode: subNode->subNodes)
				{
					if (subSubNode->isGraphNode())
					{
						subGraphNodes.emplace(subSubNode->tokenId, subSubNode);
					}
				}
			}
		}

		return subGraphNodes;
	}

	void replaceSubGraphNodes(std::map<Id, std::shared_ptr<DummyNode>> subGraphNodes) const
	{
		for (const std::shared_ptr<DummyNode>& subNode: subNodes)
		{
			if (subNode->isAccessNode())
			{
				for (size_t i = 0; i < subNode->subNodes.size(); i++)
				{
					std::shared_ptr<DummyNode> subSubNode = subNode->subNodes[i];
					if (!subSubNode->isGraphNode())
					{
						continue;
					}

					auto it = subGraphNodes.find(subSubNode->tokenId);
					if (it != subGraphNodes.end())
					{
						subNode->subNodes[i] = it->second;
						subGraphNodes.erase(it);
					}
				}
			}
		}
	}

	std::vector<std::shared_ptr<DummyNode>> getAccessNodes() const
	{
		std::vector<std::shared_ptr<DummyNode>> accessNodes;
		for (const std::shared_ptr<DummyNode>& subNode: subNodes)
		{
			if (subNode->isAccessNode())
			{
				accessNodes.push_back(subNode);
			}
		}
		return accessNodes;
	}

	void replaceAccessNodes(const std::vector<std::shared_ptr<DummyNode>>& accessNodes)
	{
		for (size_t i = 0; i < subNodes.size(); i++)
		{
			if (subNodes[i]->isAccessNode())
			{
				subNodes.erase(subNodes.begin() + i);
				i--;
			}
		}
		subNodes.insert(subNodes.end(), accessNodes.begin(), accessNodes.end());
	}

	void sortSubNodesByName()
	{
		std::sort(
			subNodes.begin(),
			subNodes.end(),
			[](const std::shared_ptr<DummyNode>& a, const std::shared_ptr<DummyNode>& b) -> bool {
				return a->name < b->name;
			});
	}

	bool getsLayouted() const
	{
		return visible && !isExpandToggleNode() && !isQualifierNode();
	}

	const DummyNode* getQualifierNode() const
	{
		for (const std::shared_ptr<DummyNode>& subNode: subNodes)
		{
			if (subNode->isQualifierNode())
			{
				return subNode.get();
			}
		}
		return nullptr;
	}

	std::vector<BundleInfo> getBundleInfos() const
	{
		std::vector<BundleInfo> bundleInfos;
		for (const std::shared_ptr<DummyNode>& subNode: subNodes)
		{
			bundleInfos.push_back(subNode->bundleInfo);
		}
		return bundleInfos;
	}

	Type type;

	Vec2i position;
	Vec2i size;

	bool visible;
	bool hidden;
	bool childVisible;

	Id tokenId;

	std::vector<std::shared_ptr<DummyNode>> subNodes;

	// GraphNode
	const Node* data;
	std::wstring name;

	bool active;
	bool connected;
	bool expanded;
	bool autoExpanded;
	bool hasParent;

	// AccessNode
	AccessKind accessKind;

	// ExpandToggleNode
	size_t invisibleSubNodeCount;

	// Bundling
	BundleInfo bundleInfo;
	Id bundleId;

	// Layout
	Vec2i columnSize;

	// BundleNode
	BundledNodesSet bundledNodes;
	size_t bundledNodeCount;
	NodeType bundledNodeType;

	// QualifierNode
	NameHierarchy qualifierName;

	// GroupNode
	GroupType groupType;
	GroupLayout groupLayout;
	std::vector<Id> hiddenEdgeIds;
	bool interactive;

	// TextNode
	int fontSizeDiff;
};

#endif	  // DUMMY_NODE_H
