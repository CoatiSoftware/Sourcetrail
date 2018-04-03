#ifndef GRAPH_CONTROLLER_H
#define GRAPH_CONTROLLER_H

#include <list>
#include <vector>

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateAll.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageActivateTrail.h"
#include "utility/messaging/type/MessageActivateTrailEdge.h"
#include "utility/messaging/type/MessageFlushUpdates.h"
#include "utility/messaging/type/MessageFocusIn.h"
#include "utility/messaging/type/MessageFocusOut.h"
#include "utility/messaging/type/MessageGraphNodeBundleSplit.h"
#include "utility/messaging/type/MessageGraphNodeExpand.h"
#include "utility/messaging/type/MessageGraphNodeHide.h"
#include "utility/messaging/type/MessageGraphNodeMove.h"
#include "utility/messaging/type/MessageScrollGraph.h"
#include "utility/messaging/type/MessageSearchFullText.h"
#include "utility/messaging/type/MessageShowErrors.h"
#include "utility/messaging/type/MessageShowReference.h"

#include "component/controller/Controller.h"
#include "component/controller/helper/DummyEdge.h"
#include "component/controller/helper/DummyNode.h"
#include "component/view/GraphView.h"
#include "data/graph/Node.h"

class Graph;
class StorageAccess;

class GraphController
	: public Controller
	, public MessageListener<MessageActivateAll>
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageActivateTrail>
	, public MessageListener<MessageActivateTrailEdge>
	, public MessageListener<MessageFlushUpdates>
	, public MessageListener<MessageFocusIn>
	, public MessageListener<MessageFocusOut>
	, public MessageListener<MessageGraphNodeBundleSplit>
	, public MessageListener<MessageGraphNodeExpand>
	, public MessageListener<MessageGraphNodeHide>
	, public MessageListener<MessageGraphNodeMove>
	, public MessageListener<MessageScrollGraph>
	, public MessageListener<MessageSearchFullText>
	, public MessageListener<MessageShowErrors>
	, public MessageListener<MessageShowReference>
{
public:
	GraphController(StorageAccess* storageAccess);
	~GraphController();

private:
	virtual void handleMessage(MessageActivateAll* message);
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageActivateTrail* message);
	virtual void handleMessage(MessageActivateTrailEdge* message);
	virtual void handleMessage(MessageFlushUpdates* message);
	virtual void handleMessage(MessageFocusIn* message);
	virtual void handleMessage(MessageFocusOut* message);
	virtual void handleMessage(MessageGraphNodeBundleSplit* message);
	virtual void handleMessage(MessageGraphNodeExpand* message);
	virtual void handleMessage(MessageGraphNodeHide* message);
	virtual void handleMessage(MessageGraphNodeMove* message);
	virtual void handleMessage(MessageScrollGraph* message);
	virtual void handleMessage(MessageSearchFullText* message);
	virtual void handleMessage(MessageShowErrors* message);
	virtual void handleMessage(MessageShowReference* message);

	GraphView* getView() const;

	virtual void clear();

	void createDummyGraph(const std::shared_ptr<Graph> graph);
	void createDummyGraphAndSetActiveAndVisibility(
		const std::vector<Id>& tokenIds, const std::shared_ptr<Graph> graph, bool keepExpandedNodesExpanded);
	std::vector<std::shared_ptr<DummyNode>> createDummyNodeTopDown(Node* node, Id ancestorId);

	void updateDummyNodeNamesAndAddQualifiers(const std::vector<std::shared_ptr<DummyNode>>& dummyNodes);

	std::vector<Id> getExpandedNodeIds() const;
	void setExpandedNodeIds(const std::vector<Id>& nodeIds);
	void autoExpandActiveNode(const std::vector<Id>& activeTokenIds);

	bool setActive(const std::vector<Id>& activeTokenIds, bool showAllEdges);
	void setVisibility(bool noActive);
	void setActiveAndVisibility(const std::vector<Id>& activeTokenIds);
	void setNodeActiveRecursive(DummyNode* node, const std::vector<Id>& activeTokenIds, bool* noActive) const;
	bool setNodeVisibilityRecursiveBottomUp(DummyNode* node, bool noActive) const;
	void setNodeVisibilityRecursiveTopDown(DummyNode* node, bool parentExpanded) const;

	void hideBuiltinTypes();

	void bundleNodes();
	void bundleNodesAndEdgesMatching(
		std::function<bool(const DummyNode::BundleInfo&, const Node*)> matcher, size_t count, bool countConnectedNodes,
		const std::wstring& name);
	std::shared_ptr<DummyNode> bundleNodesMatching(
		std::list<std::shared_ptr<DummyNode>>& nodes, std::function<bool(const DummyNode*)> matcher, const std::wstring& name);
	std::shared_ptr<DummyNode> bundleByType(
		std::list<std::shared_ptr<DummyNode>>& nodes,
		const NodeType& type,
		const Tree<NodeType::BundleInfo>& bundleInfoTree,
		const bool considerInvisibleNodes = false);
	void bundleNodesByType();

	void addCharacterIndex();
	bool hasCharacterIndex() const;

	void groupTrailNodes(NodeType::GroupType groupType);

	void layoutNesting();
	void layoutNestingRecursive(DummyNode* node) const;
	void addExpandToggleNode(DummyNode* node) const;
	void layoutToGrid(DummyNode* node) const;

	void layoutGraph(bool getSortedNodes = false);
	void layoutList();
	void layoutTrail(bool horizontal, bool hasOrigin);

	void assignBundleIds();

	std::shared_ptr<DummyNode> getDummyGraphNodeById(Id tokenId) const;
	DummyEdge* getDummyGraphEdgeById(Id tokenId) const;

	void buildGraph(MessageBase* message, bool centerActiveNode, bool animatedTransition, bool scrollToTop);

	void forEachDummyNodeRecursive(std::function<void(DummyNode*)> func);
	void forEachDummyEdge(std::function<void(DummyEdge*)> func);

	StorageAccess* m_storageAccess;

	std::vector<std::shared_ptr<DummyNode>> m_dummyNodes;
	std::vector<std::shared_ptr<DummyEdge>> m_dummyEdges;

	std::map<Id, std::shared_ptr<DummyNode>> m_dummyGraphNodes;

	std::vector<Id> m_activeNodeIds;
	std::vector<Id> m_activeEdgeIds;

	std::shared_ptr<Graph> m_graph;

	std::map<Id, Id> m_topLevelAncestorIds;

	bool m_useBezierEdges;
};

#endif // GRAPH_CONTROLLER_H
