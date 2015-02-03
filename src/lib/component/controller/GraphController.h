#ifndef GRAPH_CONTROLLER_H
#define GRAPH_CONTROLLER_H

#include <vector>

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageGraphNodeExpand.h"
#include "utility/messaging/type/MessageGraphNodeMove.h"

#include "component/controller/Controller.h"
#include "component/controller/GraphLayouter.h"
#include "component/view/GraphView.h"

struct DummyNode;
struct DummyEdge;
class Graph;
class GraphAccess;
class Node;

class GraphController
	: public Controller
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageFinishedParsing>
	, public MessageListener<MessageGraphNodeExpand>
	, public MessageListener<MessageGraphNodeMove>
{
public:
	struct Margins
	{
		Margins();

		int left;
		int right;

		int top;
		int bottom;

		int betweenX;
		int betweenY;

		int minWidth;
		float charWidth;
	};

	GraphController(GraphAccess* graphAccess);
	~GraphController();

private:
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageFinishedParsing* message);
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
	void setNodeVisibilityRecursiveTopDown(DummyNode& node) const;

	void layoutNesting();
	void layoutNestingRecursive(DummyNode& node) const;

	Margins getMarginsForDummyNode(DummyNode& node) const;
	DummyNode* findDummyNodeRecursive(std::vector<DummyNode>& nodes, Id tokenId);
	DummyNode* findDummyNodeAccessRecursive(std::vector<DummyNode>& nodes, Id parentId, TokenComponentAccess::AccessType type);

	GraphAccess* m_graphAccess;
	GraphView::Metrics m_viewMetrics;

	std::vector<DummyNode> m_dummyNodes;
	std::vector<DummyEdge> m_dummyEdges;

	std::vector<Id> m_activeTokenIds;
	std::vector<Id> m_currentActiveTokenIds;
};

#endif // GRAPH_CONTROLLER_H
