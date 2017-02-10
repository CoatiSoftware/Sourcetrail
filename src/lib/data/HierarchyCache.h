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

	void createConnection(Id edgeId, Id fromId, Id toId, bool sourceVisible, bool sourceIndexed, bool targetIndexed);

	Id getLastVisibleParentNodeId(Id nodeId) const;
	size_t getIndexOfLastVisibleParentNode(Id nodeId) const;

	void addAllChildIdsForNodeId(Id nodeId, std::set<Id>* nodeIds, std::set<Id>* edgeIds) const;
	void addAllVisibleParentsAndChildIdsForNodeId(Id nodeId, std::set<Id>* nodeIds, std::set<Id>* edgeIds) const;
	void addAllVisibleParentsRecursive(Id nodeId, std::set<Id>* nodeIds, std::set<Id>* edgeIds) const;

	void addFirstChildIdsForNodeId(Id nodeId, std::vector<Id>* nodeIds) const;
	void addFirstVisibleChildIdsForNodeId(Id nodeId, std::vector<Id>* nodeIds) const;

	bool isChildOfVisibleNodeOrInvisible(Id nodeId) const;
	bool isIndexed(Id nodeId) const;

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
		const std::vector<HierarchyNode*>& getChildren() const;

		void addChildIds(std::vector<Id>* nodeIds) const;
		void addChildIds(std::set<Id>* nodeIds, std::set<Id>* edgeIds) const;
		void addChildIdsRecursive(std::set<Id>* nodeIds, std::set<Id>* edgeIds) const;
		void addVisibleNodeIdsRecursive(std::vector<Id>* nodeIds) const;

		bool isVisible() const;
		void setIsVisible(bool isVisible);

		bool isIndexed() const;
		void setIsIndexed(bool isIndexed);

	private:
		const Id m_nodeId;
		Id m_edgeId;

		HierarchyNode* m_parent;
		std::vector<HierarchyNode*> m_children;

		bool m_isVisible;
		bool m_isIndexed;
	};

	HierarchyNode* getNode(Id nodeId) const;
	HierarchyNode* createNode(Id nodeId);

	std::map<Id, std::shared_ptr<HierarchyNode>> m_nodes;
};

#endif // HIERARCHY_CACHE_H
