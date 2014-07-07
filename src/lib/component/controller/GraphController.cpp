#include "component/controller/GraphController.h"

#include "component/view/graphElements/GraphEdge.h"
#include "component/view/graphElements/GraphNode.h"
#include "component/view/GraphView.h"
#include "data/access/GraphAccess.h"

#include "utility/logging/logging.h"

GraphController::GraphController(std::shared_ptr<GraphAccess> graphAccess)
	: m_graphAccess(graphAccess)
{
}

GraphController::~GraphController()
{
}

void GraphController::handleMessage(MessageActivateToken* message)
{
	GraphView* view = getView();
	if (view != NULL)
	{
		std::string name = m_graphAccess->getNameForNodeWithId(message->tokenId);

		DummyNode node(name, message->tokenId, Vec2i(0,0));

		std::vector<DummyNode> nodes;
		nodes.push_back(node);

		std::vector<DummyEdge> edges;

		view->rebuildGraph(nodes, edges);
	}
}

GraphView* GraphController::getView()
{
	return Controller::getView<GraphView>();
}
