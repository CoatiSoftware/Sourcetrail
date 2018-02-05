#ifndef GRAPH_LAYOUTER_H
#define GRAPH_LAYOUTER_H

#include <map>
#include <set>
#include <vector>

#include "component/controller/helper/DummyEdge.h"
#include "component/controller/helper/DummyNode.h"

// based on Sugiyama dependency graph layouting

class TrailLayouter
{
public:
	enum LayoutDirection
	{
		LAYOUT_LEFT_RIGHT,
		LAYOUT_RIGHT_LEFT,
		LAYOUT_TOP_BOTTOM,
		LAYOUT_BOTTOM_TOP
	};

	TrailLayouter(LayoutDirection dir);

	void layoutGraph(
		std::vector<std::shared_ptr<DummyNode>>& dummyNodes,
		const std::vector<std::shared_ptr<DummyEdge>>& dummyEdges,
		const std::map<Id, Id>& topLevelAncestorIds);

private:
	struct TrailEdge;

	struct TrailNode
	{
		Id id;
		int level;
		std::wstring name;

		Vec2i pos;
		Vec2i size;

		std::set<TrailEdge*> incomingEdges;
		std::set<TrailEdge*> outgoingEdges;

		DummyNode* dummyNode;
	};

	struct TrailEdge
	{
		Id id;
		TrailNode* origin;
		TrailNode* target;

		std::vector<TrailNode*> virtualNodes;

		std::vector<DummyEdge*> dummyEdges;
	};

	void buildGraph(
		std::vector<std::shared_ptr<DummyNode>>& dummyNodes,
		const std::vector<std::shared_ptr<DummyEdge>>& dummyEdges,
		const std::map<Id, Id>& topLevelAncestorIds);

	void removeDeadEnds();
	void makeAcyclicRecursive(TrailNode* node, std::set<TrailNode*> predecessors);

	void assignLongestPathLevels();
	void assignRemainingLevels();

	void addVirtualNodes();
	void buildColumns();
	void reduceEdgeCrossings();

	void layout();
	void moveNodesToAveragePosition(std::vector<TrailNode*> nodes, bool forward);
	void retrievePositions(const std::map<Id, Id>& topLevelAncestorIds);

	void print();

	void addNode(const std::shared_ptr<DummyNode>& dummyNode);
	void addEdge(const std::shared_ptr<DummyEdge> dummyEdge, const std::map<Id, Id>& topLevelAncestorIds);
	void switchEdge(TrailEdge* edge);

	bool horizontalLayout() const;
	bool invertedLayout() const;

	LayoutDirection m_direction;

	std::vector<std::shared_ptr<TrailNode>> m_allNodes;
	std::vector<std::shared_ptr<TrailEdge>> m_allEdges;

	std::map<Id, TrailNode*> m_nodesById;
	TrailNode* m_rootNode;

	std::vector<std::vector<TrailNode*>> m_nodesPerCol;
};

#endif // GRAPH_LAYOUTER_H
