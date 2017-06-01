#include "data/HierarchyCache.h"

HierarchyCache::HierarchyNode::HierarchyNode(Id nodeId)
	: m_nodeId(nodeId)
	, m_edgeId(0)
	, m_parent(nullptr)
	, m_isVisible(true)
	, m_isImplicit(false)
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

size_t HierarchyCache::HierarchyNode::getChildrenCount() const
{
	return m_children.size();
}

size_t HierarchyCache::HierarchyNode::getNonImplicitChildrenCount() const
{
	size_t count = 0;
	for (const HierarchyNode* child : m_children)
	{
		if (!child->isImplicit())
		{
			count++;
		}
	}
	return count;
}

void HierarchyCache::HierarchyNode::addNonImplicitChildIds(std::vector<Id>* nodeIds, std::vector<Id>* edgeIds) const
{
	for (const HierarchyNode* child : m_children)
	{
		if (!child->isImplicit())
		{
			nodeIds->push_back(child->getNodeId());
			edgeIds->push_back(child->getEdgeId());
		}
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

bool HierarchyCache::HierarchyNode::isVisible() const
{
	return m_isVisible;
}

void HierarchyCache::HierarchyNode::setIsVisible(bool isVisible)
{
	m_isVisible = isVisible;
}

bool HierarchyCache::HierarchyNode::isImplicit() const
{
	return m_isImplicit;
}

void HierarchyCache::HierarchyNode::setIsImplicit(bool isImplicit)
{
	m_isImplicit = isImplicit;
}


void HierarchyCache::clear()
{
	m_nodes.clear();
}

void HierarchyCache::createConnection(Id edgeId, Id fromId, Id toId, bool sourceVisible, bool targetImplicit)
{
	HierarchyNode* from = createNode(fromId);
	HierarchyNode* to = createNode(toId);

	from->addChild(to);
	to->setParent(from);

	from->setIsVisible(sourceVisible);

	to->setEdgeId(edgeId);
	to->setIsImplicit(targetImplicit);
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

void HierarchyCache::addAllVisibleParentIdsForNodeId(Id nodeId, std::set<Id>* nodeIds, std::set<Id>* edgeIds) const
{
	HierarchyNode* node = getNode(nodeId);
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

void HierarchyCache::addAllChildIdsForNodeId(Id nodeId, std::set<Id>* nodeIds, std::set<Id>* edgeIds) const
{
	HierarchyNode* node = getNode(nodeId);
	if (node && node->isVisible())
	{
		node->addChildIdsRecursive(nodeIds, edgeIds);
	}
}

void HierarchyCache::addFirstNonImplicitChildIdsForNodeId(Id nodeId, std::vector<Id>* nodeIds, std::vector<Id>* edgeIds) const
{
	HierarchyNode* node = getNode(nodeId);
	if (node)
	{
		node->addNonImplicitChildIds(nodeIds, edgeIds);
	}
}

size_t HierarchyCache::getFirstNonImplicitChildIdsCountForNodeId(Id nodeId) const
{
	HierarchyNode* node = getNode(nodeId);
	if (node)
	{
		return node->getNonImplicitChildrenCount();
	}
	return 0;
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

bool HierarchyCache::nodeHasChildren(Id nodeId) const
{
	HierarchyNode* node = getNode(nodeId);
	if (node)
	{
		return node->getChildrenCount();
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
