#include "HierarchyCache.h"

#include "utility.h"

HierarchyCache::HierarchyNode::HierarchyNode(Id nodeId)
	: m_nodeId(nodeId), m_edgeId(0), m_parent(nullptr), m_isVisible(true), m_isImplicit(false)
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

void HierarchyCache::HierarchyNode::addBase(HierarchyNode* base, Id edgeId)
{
	m_bases.push_back(base);
	m_baseEdgeIds.push_back(edgeId);
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
	for (const HierarchyNode* child: m_children)
	{
		if (!child->isImplicit())
		{
			count++;
		}
	}
	return count;
}

void HierarchyCache::HierarchyNode::addChildIds(std::vector<Id>* nodeIds, std::vector<Id>* edgeIds) const
{
	for (const HierarchyNode* child: m_children)
	{
		nodeIds->push_back(child->getNodeId());
		edgeIds->push_back(child->getEdgeId());
	}
}

void HierarchyCache::HierarchyNode::addNonImplicitChildIds(
	std::vector<Id>* nodeIds, std::vector<Id>* edgeIds) const
{
	for (const HierarchyNode* child: m_children)
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
	for (const HierarchyNode* child: m_children)
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

void HierarchyCache::HierarchyNode::addInheritanceEdgesRecursive(
	Id startId,
	const std::set<Id>& inheritanceEdgeIds,
	const std::set<Id>& nodeIds,
	std::vector<std::tuple<Id, Id, std::vector<Id>>>* inheritanceEdges)
{
	for (size_t i = 0; i < m_bases.size(); i++)
	{
		if (inheritanceEdgeIds.find(m_baseEdgeIds[i]) != inheritanceEdgeIds.end())
		{
			continue;
		}

		HierarchyNode* base = m_bases[i];
		Id baseId = base->getNodeId();

		std::set<Id> inheritanceEdgeIds2 = inheritanceEdgeIds;
		inheritanceEdgeIds2.insert(m_baseEdgeIds[i]);

		if (nodeIds.find(baseId) != nodeIds.end())
		{
			inheritanceEdges->push_back({startId, baseId, utility::toVector(inheritanceEdgeIds2)});
		}

		base->addInheritanceEdgesRecursive(startId, inheritanceEdgeIds2, nodeIds, inheritanceEdges);
	}
}


void HierarchyCache::clear()
{
	m_nodes.clear();
}

void HierarchyCache::createConnection(
	Id edgeId, Id fromId, Id toId, bool sourceVisible, bool sourceImplicit, bool targetImplicit)
{
	if (fromId == toId)
	{
		return;
	}

	HierarchyNode* from = createNode(fromId);
	HierarchyNode* to = createNode(toId);

	from->addChild(to);
	to->setParent(from);

	from->setIsVisible(sourceVisible);
	from->setIsImplicit(sourceImplicit);

	to->setEdgeId(edgeId);
	to->setIsImplicit(targetImplicit);
}

void HierarchyCache::createInheritance(Id edgeId, Id fromId, Id toId)
{
	if (fromId == toId)
	{
		return;
	}

	HierarchyNode* from = createNode(fromId);
	HierarchyNode* to = createNode(toId);

	from->addBase(to, edgeId);
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

		if (node->isVisible() && !idx)
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

void HierarchyCache::addAllVisibleParentIdsForNodeId(
	Id nodeId, std::set<Id>* nodeIds, std::set<Id>* edgeIds) const
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

void HierarchyCache::addFirstChildIdsForNodeId(
	Id nodeId, std::vector<Id>* nodeIds, std::vector<Id>* edgeIds) const
{
	HierarchyNode* node = getNode(nodeId);
	if (node)
	{
		if (node->isImplicit())
		{
			node->addChildIds(nodeIds, edgeIds);
		}
		else
		{
			node->addNonImplicitChildIds(nodeIds, edgeIds);
		}
	}
}

size_t HierarchyCache::getFirstChildIdsCountForNodeId(Id nodeId) const
{
	HierarchyNode* node = getNode(nodeId);
	if (node)
	{
		if (node->isImplicit())
		{
			return node->getChildrenCount();
		}
		else
		{
			return node->getNonImplicitChildrenCount();
		}
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

bool HierarchyCache::nodeIsVisible(Id nodeId) const
{
	HierarchyNode* node = getNode(nodeId);
	if (node)
	{
		return node->isVisible();
	}

	return false;
}

bool HierarchyCache::nodeIsImplicit(Id nodeId) const
{
	HierarchyNode* node = getNode(nodeId);
	if (node)
	{
		return node->isImplicit();
	}

	return false;
}

std::vector<std::tuple<Id, Id, std::vector<Id>>> HierarchyCache::getInheritanceEdgesForNodeId(
	Id nodeId, const std::set<Id>& nodeIds) const
{
	std::vector<std::tuple<Id, Id, std::vector<Id>>> inheritanceEdges;

	HierarchyNode* node = getNode(nodeId);
	if (node)
	{
		node->addInheritanceEdgesRecursive(node->getNodeId(), {}, nodeIds, &inheritanceEdges);
	}

	return inheritanceEdges;
}

HierarchyCache::HierarchyNode* HierarchyCache::getNode(Id nodeId) const
{
	auto it = m_nodes.find(nodeId);

	if (it != m_nodes.end())
	{
		return it->second.get();
	}

	return nullptr;
}

HierarchyCache::HierarchyNode* HierarchyCache::createNode(Id nodeId)
{
	auto it = m_nodes.find(nodeId);

	if (it == m_nodes.end())
	{
		it = m_nodes.emplace(nodeId, std::make_unique<HierarchyNode>(nodeId)).first;
	}

	return it->second.get();
}
