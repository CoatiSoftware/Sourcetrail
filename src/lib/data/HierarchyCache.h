#ifndef HIERARCHY_CACHE_H
#define HIERARCHY_CACHE_H

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "utility/types.h"

class HierarchyCache
{
public:
	void clear();

	void createConnection(Id edgeId, Id fromId, Id toId, bool sourceVisible, bool targetImplicit);

	Id getLastVisibleParentNodeId(Id nodeId) const;
	size_t getIndexOfLastVisibleParentNode(Id nodeId) const;

	void addAllVisibleParentIdsForNodeId(Id nodeId, std::set<Id>* nodeIds, std::set<Id>* edgeIds) const;

	void addAllChildIdsForNodeId(Id nodeId, std::set<Id>* nodeIds, std::set<Id>* edgeIds) const;
	void addFirstNonImplicitChildIdsForNodeId(Id nodeId, std::vector<Id>* nodeIds, std::vector<Id>* edgeIds) const;

	size_t getFirstNonImplicitChildIdsCountForNodeId(Id nodeId) const;

	bool isChildOfVisibleNodeOrInvisible(Id nodeId) const;

	bool nodeHasChildren(Id nodeId) const;

private:
	class HierarchyNode
	{
	public:
		HierarchyNode(Id nodeId);

		Id getNodeId() const;

		Id getEdgeId() const;
		void setEdgeId(Id edgeId);

		HierarchyNode* getParent() const;
		void setParent(HierarchyNode* parent);

		void addChild(HierarchyNode* child);

		size_t getChildrenCount() const;
		size_t getNonImplicitChildrenCount() const;

		void addNonImplicitChildIds(std::vector<Id>* nodeIds, std::vector<Id>* edgeIds) const;
		void addChildIdsRecursive(std::set<Id>* nodeIds, std::set<Id>* edgeIds) const;

		bool isVisible() const;
		void setIsVisible(bool isVisible);

		bool isImplicit() const;
		void setIsImplicit(bool isImplicit);

	private:
		const Id m_nodeId;
		Id m_edgeId;

		HierarchyNode* m_parent;
		std::vector<HierarchyNode*> m_children;

		bool m_isVisible;
		bool m_isImplicit;
	};

	HierarchyNode* getNode(Id nodeId) const;
	HierarchyNode* createNode(Id nodeId);

	std::map<Id, std::shared_ptr<HierarchyNode>> m_nodes;
};

#endif // HIERARCHY_CACHE_H
