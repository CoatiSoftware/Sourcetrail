#include "data/HierarchyCache.h"

HierarchyCache::HierarchyNode::HierarchyNode(Id nodeId)
	: m_nodeId(nodeId)
	, m_edgeId(0)
	, m_parent(nullptr)
	, m_isVisible(true)
	, m_isIndexed(true)
{
}

Id HierarchyCache::HierarchyNode::getNodeId() const
{
	return m_nodeId;
}

Id HierarchyCache::HierarchyNode::getEdgeId() const
{
	return m_edgeId;
}

void HierarchyCache::HierarchyNode::setEdgeId(Id edgeId)
{
	m_edgeId = edgeId;
}

HierarchyCache::HierarchyNode* HierarchyCache::HierarchyNode::getParent() const
{
	return m_parent;
}

void HierarchyCache::HierarchyNode::setParent(HierarchyNode* parent)
{
	m_parent = parent;
}

void HierarchyCache::HierarchyNode::addChild(HierarchyNode* child)
{
	m_children.push_back(child);
}

const std::vector<HierarchyCache::HierarchyNode*>& HierarchyCache::HierarchyNode::getChildren() const
{
	return m_children;
}

void HierarchyCache::HierarchyNode::addChildIds(std::vector<Id>* nodeIds) const
{
	for (const HierarchyNode* child : m_children)
	{
		nodeIds->push_back(child->getNodeId());
	}
}

void HierarchyCache::HierarchyNode::addChildIds(std::set<Id>* nodeIds, std::set<Id>* edgeIds) const
{
	for (const HierarchyNode* child : m_children)
	{
		nodeIds->insert(child->getNodeId());
		edgeIds->insert(child->getEdgeId());
	}
}

void HierarchyCache::HierarchyNode::addChildIdsRecursive(std::set<Id>* nodeIds, std::set<Id>* edgeIds) const
{
	for (const HierarchyNode* child : m_children)
	{
		nodeIds->insert(child->getNodeId());
		edgeIds->insert(child->getEdgeId());

		child->addChildIdsRecursive(nodeIds, edgeIds);
	}
}

void HierarchyCache::HierarchyNode::addVisibleNodeIdsRecursive(std::vector<Id>* nodeIds) const
{
	if (isVisible())
	{
		nodeIds->push_back(getNodeId());
	}
	else
	{
		for (const HierarchyNode* child : m_children)
		{
			child->addVisibleNodeIdsRecursive(nodeIds);
		}
	}
}

bool HierarchyCache::HierarchyNode::isVisible() const
{
	return m_isVisible;
}

void HierarchyCache::HierarchyNode::setIsVisible(bool isVisible)
{
	m_isVisible = isVisible;
}

bool HierarchyCache::HierarchyNode::isIndexed() const
{
	return m_isIndexed;
}

void HierarchyCache::HierarchyNode::setIsIndexed(bool isIndexed)
{
	m_isIndexed = isIndexed;
}


void HierarchyCache::clear()
{
	m_nodes.clear();
}

void HierarchyCache::createConnection(Id edgeId, Id fromId, Id toId, bool sourceVisible, bool sourceIndexed, bool targetIndexed)
{
	HierarchyNode* from = createNode(fromId);
	HierarchyNode* to = createNode(toId);

	from->addChild(to);
	to->setParent(from);

	from->setIsVisible(sourceVisible);
	from->setIsIndexed(sourceIndexed);

	to->setEdgeId(edgeId);
	to->setIsIndexed(targetIndexed);
}

Id HierarchyCache::getLastVisibleParentNodeId(Id nodeId) const
{
	HierarchyNode* node = nullptr;
	HierarchyNode* parent = getNode(nodeId);

	while (parent && parent->isVisible())
	{
		node = parent;
		parent = node->getParent();

		nodeId = node->getNodeId();
	}

	return nodeId;
}

size_t HierarchyCache::getIndexOfLastVisibleParentNode(Id nodeId) const
{
	HierarchyNode* node = nullptr;
	HierarchyNode* parent = getNode(nodeId);

	size_t idx = 0;
	bool visible = false;

	while (parent)
	{
		node = parent;
		parent = node->getParent();

		if (node->isVisible())
		{
			visible = true;
		}
		else if (visible)
		{
			idx++;
		}
	}

	return idx;
}

void HierarchyCache::addAllChildIdsForNodeId(Id nodeId, std::set<Id>* nodeIds, std::set<Id>* edgeIds) const
{
	HierarchyNode* node = getNode(nodeId);
	if (node && node->isVisible())
	{
		node->addChildIdsRecursive(nodeIds, edgeIds);
	}
}

void HierarchyCache::addAllVisibleParentsAndChildIdsForNodeId(Id nodeId, std::set<Id>* nodeIds, std::set<Id>* edgeIds) const
{
	HierarchyNode* node = getNode(nodeId);
	if (node && node->isVisible())
	{
		node->addChildIds(nodeIds, edgeIds);
	}

	Id edgeId = 0;
	while (node && node->isVisible())
	{
		if (edgeId)
		{
			edgeIds->insert(edgeId);
		}

		nodeIds->insert(node->getNodeId());
		edgeId = node->getEdgeId();

		node = node->getParent();
	}
}

void HierarchyCache::addFirstChildIdsForNodeId(Id nodeId, std::vector<Id>* nodeIds) const
{
	HierarchyNode* node = getNode(nodeId);
	if (node)
	{
		node->addChildIds(nodeIds);
	}
}

void HierarchyCache::addFirstVisibleChildIdsForNodeId(Id nodeId, std::vector<Id>* nodeIds) const
{
	HierarchyNode* node = getNode(nodeId);
	if (node)
	{
		node->addVisibleNodeIdsRecursive(nodeIds);
	}
	else
	{
		nodeIds->push_back(nodeId);
	}
}

bool HierarchyCache::isChildOfVisibleNodeOrInvisible(Id nodeId) const
{
	HierarchyNode* node = getNode(nodeId);
	if (!node)
	{
		return false;
	}

	if (!node->isVisible())
	{
		return true;
	}

	if (node->getParent() && node->getParent()->isVisible())
	{
		return true;
	}

	return false;
}

bool HierarchyCache::isIndexed(Id nodeId) const
{
	HierarchyNode* node = getNode(nodeId);
	if (!node)
	{
		return true;
	}

	return node->isIndexed();
}

bool HierarchyCache::nodeHasChildren(Id nodeId) const
{
	HierarchyNode* node = getNode(nodeId);
	if (node)
	{
		return node->getChildren().size();
	}

	return false;
}

HierarchyCache::HierarchyNode* HierarchyCache::getNode(Id nodeId) const
{
	std::map<Id, std::shared_ptr<HierarchyNode>>::const_iterator it = m_nodes.find(nodeId);

	if (it != m_nodes.end())
	{
		return it->second.get();
	}

	return nullptr;
}

HierarchyCache::HierarchyNode* HierarchyCache::createNode(Id nodeId)
{
	std::map<Id, std::shared_ptr<HierarchyNode>>::iterator it = m_nodes.find(nodeId);

	if (it == m_nodes.end())
	{
		it = m_nodes.emplace(nodeId, std::make_shared<HierarchyNode>(nodeId)).first;
	}

	return it->second.get();
}
