#include "component/controller/GraphController.h"

#include <set>

#include "utility/logging/logging.h"

#include "component/view/graphElements/GraphEdge.h"
#include "component/view/graphElements/GraphNode.h"
#include "component/view/GraphView.h"
#include "data/access/GraphAccess.h"

GraphController::GraphController(GraphAccess* graphAccess)
	: m_graphAccess(graphAccess)
{
}

GraphController::~GraphController()
{
}

void GraphController::handleMessage(MessageActivateToken* message)
{
	createDummyGraphForTokenId(message->tokenId, &GraphLayouter::layoutSimpleRing);
}

void GraphController::handleMessage(MessageActivateTokens* message)
{
	//createDummyGraphForTokenId(message->tokenIds[0], &GraphLayouter::layoutSimpleRing);
}

GraphView* GraphController::getView()
{
	return Controller::getView<GraphView>();
}

void GraphController::createDummyGraphForTokenId(Id tokenId, const GraphLayouter::LayoutFunction layoutFunction)
{
	GraphView* view = getView();
	if (!view)
	{
		LOG_ERROR("GraphController has no associated GraphView");
		return;
	}

	std::vector<Id> activeTokenIds;
	activeTokenIds.push_back(tokenId);
	std::shared_ptr<Graph> graph = m_graphAccess->getGraphForActiveTokenIds(activeTokenIds);

	std::vector<DummyNode> dummyNodes;
	std::vector<DummyEdge> dummyEdges;
	std::set<Id> addedNodes;

	graph->forEachNode(
		[&dummyNodes, &dummyEdges, &addedNodes, this](Node* node)
		{
			Node* parent = node->getLastParentNode();
			Id id = parent->getId();
			if (addedNodes.find(id) != addedNodes.end())
			{
				return;
			}
			addedNodes.insert(id);

			dummyNodes.push_back(createDummyNodeTopDown(parent, &dummyEdges));
		}
	);

	layoutFunction(dummyNodes);
	view->rebuildGraph(dummyNodes, dummyEdges);
}

DummyNode GraphController::createDummyNodeTopDown(Node* node, std::vector<DummyEdge>* dummyEdges) const
{
	DummyNode result("invalid", 0, Vec2i());

	result.name = node->getName();
	result.tokenId = node->getId();

	node->forEachChildNode(
		[&result, dummyEdges, this](Node* child)
		{
			result.subNodes.push_back(createDummyNodeTopDown(child, dummyEdges));
		}
	);

	node->forEachEdge(
		[&result, node, dummyEdges](Edge* edge)
		{
			if (edge->isType(Edge::EDGE_MEMBER))
			{
				return;
			}

			for (const DummyEdge& dummy : *dummyEdges)
			{
				if (dummy.tokenId == edge->getId())
				{
					return;
				}
			}

			dummyEdges->push_back(
				DummyEdge(edge->getFrom()->getId(), edge->getTo()->getId(), edge->getId(), edge->getType()));
		}
	);

	return result;
}
