#ifndef GRAPH_CONTROLLER_H
#define GRAPH_CONTROLLER_H

#include <list>
#include <vector>

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateAll.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageFlushUpdates.h"
#include "utility/messaging/type/MessageFocusIn.h"
#include "utility/messaging/type/MessageFocusOut.h"
#include "utility/messaging/type/MessageGraphNodeBundleSplit.h"
#include "utility/messaging/type/MessageGraphNodeExpand.h"
#include "utility/messaging/type/MessageGraphNodeMove.h"
#include "utility/messaging/type/MessageShowErrors.h"

#include "component/controller/Controller.h"
#include "component/view/GraphView.h"
#include "data/graph/token_component/TokenComponentAccess.h"
#include "data/graph/token_component/TokenComponentAggregation.h"

struct DummyNode;
struct DummyEdge;
class Graph;
class Node;
class StorageAccess;

class GraphController
	: public Controller
	, public MessageListener<MessageActivateAll>
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageFlushUpdates>
	, public MessageListener<MessageFocusIn>
	, public MessageListener<MessageFocusOut>
	, public MessageListener<MessageGraphNodeBundleSplit>
	, public MessageListener<MessageGraphNodeExpand>
	, public MessageListener<MessageGraphNodeMove>
	, public MessageListener<MessageShowErrors>
{
public:
	GraphController(StorageAccess* storageAccess);
	~GraphController();

private:
	virtual void handleMessage(MessageActivateAll* message);
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageFlushUpdates* message);
	virtual void handleMessage(MessageFocusIn* message);
	virtual void handleMessage(MessageFocusOut* message);
	virtual void handleMessage(MessageGraphNodeBundleSplit* message);
	virtual void handleMessage(MessageGraphNodeExpand* message);
	virtual void handleMessage(MessageGraphNodeMove* message);
	virtual void handleMessage(MessageShowErrors* message);

	GraphView* getView() const;

	void clear();

	void createDummyGraphForTokenIds(const std::vector<Id>& tokenIds, const std::shared_ptr<Graph> graph);
	DummyNode createDummyNodeTopDown(Node* node);

	void autoExpandActiveNode(const std::vector<Id>& activeTokenIds);

	void setActiveAndVisibility(const std::vector<Id>& activeTokenIds);
	void setNodeActiveRecursive(DummyNode& node, const std::vector<Id>& activeTokenIds) const;
	void removeImplicitAndUndefinedChildrenRecursive(DummyNode& node);
	bool setNodeVisibilityRecursiveBottomUp(DummyNode& node, bool noActive) const;
	void setNodeVisibilityRecursiveTopDown(DummyNode& node, bool parentExpanded) const;
	void deactivateNodesRecursive(std::vector<DummyNode>* nodes) const;

	void bundleNodes();
	void bundleNodesAndEdgesMatching(std::function<bool(const DummyNode&)> matcher, size_t count, const std::string& name);
	void bundleNodesMatching(std::list<DummyNode*>& nodes, std::function<bool(const DummyNode&)> matcher, const std::string& name);
	bool isTypeNodeWithSingleAggregation(const DummyNode& node, TokenComponentAggregation::Direction direction) const;
	bool isTypeNodeWithSingleInheritance(const DummyNode& node, bool isBase) const;
	bool isUndefinedNode(const DummyNode& node, bool isUsed) const;
	bool isTypeUserNode(const DummyNode& node) const;
	void bundleNodesByType();

	void layoutNesting();
	void layoutNestingRecursive(DummyNode& node) const;
	void addExpandToggleNode(DummyNode& node) const;
	void layoutToGrid(DummyNode& node) const;

	void layoutGraph();

	DummyNode* findDummyNodeRecursive(std::vector<DummyNode>& nodes, Id tokenId) const;
	DummyNode* findTopLevelDummyNodeRecursive(std::vector<DummyNode>& nodes, Id tokenId) const;
	DummyNode* findDummyNodeAccessRecursive(std::vector<DummyNode>& nodes, Id parentId, TokenComponentAccess::AccessType type) const;

	void buildGraph(MessageBase* message);

	StorageAccess* m_storageAccess;

	std::vector<DummyNode> m_dummyNodes;
	std::vector<DummyEdge> m_dummyEdges;

	std::vector<Id> m_activeNodeIds;
	std::vector<Id> m_activeEdgeIds;

	std::shared_ptr<Graph> m_graph;
};

#endif // GRAPH_CONTROLLER_H
