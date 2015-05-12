#ifndef GRAPH_CONTROLLER_H
#define GRAPH_CONTROLLER_H

#include <vector>

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageFocusIn.h"
#include "utility/messaging/type/MessageFocusOut.h"
#include "utility/messaging/type/MessageGraphNodeExpand.h"
#include "utility/messaging/type/MessageGraphNodeMove.h"

#include "component/controller/Controller.h"
#include "component/controller/GraphLayouter.h"
#include "component/view/GraphView.h"
#include "data/graph/token_component/TokenComponentAccess.h"

struct DummyNode;
struct DummyEdge;
class Graph;
class Node;
class StorageAccess;

class GraphController
	: public Controller
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageFinishedParsing>
	, public MessageListener<MessageFocusIn>
	, public MessageListener<MessageFocusOut>
	, public MessageListener<MessageGraphNodeExpand>
	, public MessageListener<MessageGraphNodeMove>
{
public:
	GraphController(StorageAccess* storageAccess);
	~GraphController();

private:
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageFinishedParsing* message);
	virtual void handleMessage(MessageFocusIn* message);
	virtual void handleMessage(MessageFocusOut* message);
	virtual void handleMessage(MessageGraphNodeExpand* message);
	virtual void handleMessage(MessageGraphNodeMove* message);

	GraphView* getView() const;

	void setActiveTokenIds(const std::vector<Id>& activeTokenIds);

	void createDummyGraphForTokenIds(const std::vector<Id>& tokenIds);
	DummyNode createDummyNodeTopDown(Node* node);

	void autoExpandActiveNode(const std::vector<Id>& activeTokenIds);

	void setActiveAndVisibility(const std::vector<Id>& activeTokenIds);
	void setNodeActiveRecursive(DummyNode& node, const std::vector<Id>& activeTokenIds) const;
	bool setNodeVisibilityRecursiveBottomUp(DummyNode& node, bool aggregated) const;
	void setNodeVisibilityRecursiveTopDown(DummyNode& node, bool parentExpanded) const;

	void layoutNesting();
	void layoutNestingRecursive(DummyNode& node) const;
	void addExpandToggleNode(DummyNode& node) const;
	void layoutToGrid(DummyNode& node) const;

	DummyNode* findDummyNodeRecursive(std::vector<DummyNode>& nodes, Id tokenId);
	DummyNode* findDummyNodeAccessRecursive(std::vector<DummyNode>& nodes, Id parentId, TokenComponentAccess::AccessType type);

	StorageAccess* m_storageAccess;

	std::vector<DummyNode> m_dummyNodes;
	std::vector<DummyEdge> m_dummyEdges;

	std::vector<Id> m_activeTokenIds;
	std::vector<Id> m_currentActiveTokenIds;
};

#endif // GRAPH_CONTROLLER_H
