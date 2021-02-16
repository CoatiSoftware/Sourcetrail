#ifndef HIERARCHY_CACHE_H
#define HIERARCHY_CACHE_H

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "types.h"

class HierarchyCache
{
public:
	void clear();

	void createConnection(
		Id edgeId, Id fromId, Id toId, bool sourceVisible, bool sourceImplicit, bool targetImplicit);
	void createInheritance(Id edgeId, Id fromId, Id toId);

	Id getLastVisibleParentNodeId(Id nodeId) const;
	size_t getIndexOfLastVisibleParentNode(Id nodeId) const;

	void addAllVisibleParentIdsForNodeId(Id nodeId, std::set<Id>* nodeIds, std::set<Id>* edgeIds) const;

	void addAllChildIdsForNodeId(Id nodeId, std::set<Id>* nodeIds, std::set<Id>* edgeIds) const;
	void addFirstChildIdsForNodeId(Id nodeId, std::vector<Id>* nodeIds, std::vector<Id>* edgeIds) const;

	size_t getFirstChildIdsCountForNodeId(Id nodeId) const;

	bool isChildOfVisibleNodeOrInvisible(Id nodeId) const;

	bool nodeHasChildren(Id nodeId) const;
	bool nodeIsVisible(Id nodeId) const;
	bool nodeIsImplicit(Id nodeId) const;

	std::vector<std::tuple</*source*/ Id, /*target*/ Id, std::vector</*edge*/ Id>>>
		getInheritanceEdgesForNodeId(Id sourceId, const std::set<Id>& targetIds) const;

private:
	/**
	 * Determine nodes and edges from which a specific node can be reached in a reversed graph.
	 *
	 * A reversed graph can be produced by HierarchyNode::getReverseReachableInheritanceSubgraph().
	 *
	 * @param[in]  nodeId        ID of the target node.
	 * @param[in]  reverseGraph  The reversed graph.
	 * @param[out] nodes         The nodes from which the node @p nodeId can be reached.
	 * @param[out] edges         The edges from which the node @p nodeId can be reached.
	 *
	 * @pre The arguments for @p nodes and @p edges must be provided empty.
	 */
	static void getReverseReachable(
		Id nodeId,
		const std::map</*target*/ Id, std::vector<std::pair</*source*/ Id, /*edge*/ Id>>>&
			reverseGraph,
		std::set<Id>& nodes,
		std::vector<Id>& edges);

	class HierarchyNode
	{
	public:
		HierarchyNode(Id nodeId);

		Id getNodeId() const;

		Id getEdgeId() const;
		void setEdgeId(Id edgeId);

		HierarchyNode* getParent() const;
		void setParent(HierarchyNode* parent);

		void addBase(HierarchyNode* base, Id edgeId);

		void addChild(HierarchyNode* child);

		size_t getChildrenCount() const;
		size_t getNonImplicitChildrenCount() const;

		void addChildIds(std::vector<Id>* nodeIds, std::vector<Id>* edgeIds) const;
		void addNonImplicitChildIds(std::vector<Id>* nodeIds, std::vector<Id>* edgeIds) const;
		void addChildIdsRecursive(std::set<Id>* nodeIds, std::set<Id>* edgeIds) const;

		bool isVisible() const;
		void setIsVisible(bool isVisible);

		bool isImplicit() const;
		void setIsImplicit(bool isImplicit);

		/**
		 * Determine the reversed subgraph of all nodes and edges that are reachable from this node.
		 *
		 * The subgraph is represented by a map that maps a node ID *t* to a set of pairs where each
		 * pair consists of a node ID *s* and and edge ID *e* such that *e* refers to an edge from
		 * *s* to *t*. Note that the mapping is reversed compared to the edges.
		 */
		std::map</*target*/ Id, std::vector<std::pair</*source*/ Id, /*edge*/ Id>>>
			getReverseReachableInheritanceSubgraph() const;

	private:
		/**
		 * Helper for getReverseReachableInheritanceSubgraph().
		 */
		void getReverseReachableInheritanceSubgraphHelper(
			std::map</*target*/ Id, std::vector<std::pair</*source*/ Id, /*edge*/ Id>>>&) const;

		const Id m_nodeId;
		Id m_edgeId;

		HierarchyNode* m_parent;

		std::vector<HierarchyNode*> m_bases;
		std::vector<Id> m_baseEdgeIds;

		std::vector<HierarchyNode*> m_children;

		bool m_isVisible;
		bool m_isImplicit;
	};

	HierarchyNode* getNode(Id nodeId) const;
	HierarchyNode* createNode(Id nodeId);

	std::map<Id, std::unique_ptr<HierarchyNode>> m_nodes;
};

#endif	  // HIERARCHY_CACHE_H
