#ifndef HIERARCHY_CACHE_H
#define HIERARCHY_CACHE_H

#include <map>
#include <memory>
#include <vector>

#include "utility/types.h"

class HierarchyCache
{
public:
	void clear();

	void createConnection(Id edgeId, Id fromId, Id toId, bool fromVisible);

	Id getLastVisibleParentNodeId(Id nodeId) const;
	void addAllChildIdsForNodeId(Id nodeId, std::vector<Id>* nodeIds, std::vector<Id>* edgeIds) const;

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
		void addChildIdsRecursive(std::vector<Id>* nodeIds, std::vector<Id>* edgeIds) const;

		bool isVisible() const;
		void setIsVisible(bool isVisible);

	private:
		const Id m_nodeId;
		Id m_edgeId;

		HierarchyNode* m_parent;
		std::vector<HierarchyNode*> m_children;

		bool m_isVisible;
	};

	HierarchyNode* getNode(Id nodeId) const;
	HierarchyNode* createNode(Id nodeId);

	std::map<Id, std::shared_ptr<HierarchyNode>> m_nodes;
};

#endif // HIERARCHY_CACHE_H
