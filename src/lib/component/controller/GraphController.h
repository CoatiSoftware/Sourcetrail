#ifndef GRAPH_CONTROLLER_H
#define GRAPH_CONTROLLER_H

#include <vector>

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateToken.h"
#include "utility/messaging/type/MessageActivateTokens.h"

#include "component/controller/Controller.h"
#include "component/controller/GraphLayouter.h"

struct DummyNode;
struct DummyEdge;
class Graph;
class GraphView;
class GraphAccess;
class Node;

class GraphController
	: public Controller
	, public MessageListener<MessageActivateToken>
	, public MessageListener<MessageActivateTokens>
{
public:
	GraphController(GraphAccess* graphAccess);
	~GraphController();

private:
	virtual void handleMessage(MessageActivateToken* message);
	virtual void handleMessage(MessageActivateTokens* message);

	GraphView* getView();

	void createDummyGraphForTokenIds(const std::vector<Id>& tokenIds);
	DummyNode createDummyNodeTopDown(Node* node, std::vector<DummyEdge>* dummyEdges) const;

	GraphAccess* m_graphAccess;
};

#endif // GRAPH_CONTROLLER_H
