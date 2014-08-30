#ifndef GRAPH_CONTROLLER_H
#define GRAPH_CONTROLLER_H

#include <set>
#include <vector>

#include "component/controller/Controller.h"
#include "component/controller/GraphLayouter.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateToken.h"
#include "utility/messaging/type/MessageActivateTokens.h"

struct DummyNode;
struct DummyEdge;
class GraphView;
class GraphAccess;

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
	void createDummyGraph(const Id activeId, const LayoutFunction layoutFunction);

	DummyNode createDummyNode(const Id nodeId);
	Id findTopLevelNode(const Id nodeId);
	DummyNode buildNodeTopDown(const Id nodeId);

	std::vector<DummyNode> createNeighbourNodes(const Id nodeId);
	std::vector<DummyNode> createNeighbourNodes(const DummyNode& node);

	std::vector<DummyEdge> createEdges(const std::vector<DummyNode>& nodes);
	std::set<DummyEdge> getNeighbourEdgesOfNode(const DummyNode& node);

	GraphAccess* m_graphAccess;
};

#endif // GRAPH_CONTROLLER_H
