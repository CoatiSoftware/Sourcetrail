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
	std::vector<Id> activeTokenIds(1, message->tokenId);
	createDummyGraphForTokenIds(activeTokenIds);
}

void GraphController::handleMessage(MessageActivateTokens* message)
{
	createDummyGraphForTokenIds(message->tokenIds);
}

GraphView* GraphController::getView()
{
	return Controller::getView<GraphView>();
}

void GraphController::createDummyGraphForTokenIds(const std::vector<Id>& tokenIds)
{
	const GraphLayouter::LayoutFunction layoutFunction = &GraphLayouter::layoutSimpleRing;

	GraphView* view = getView();
	if (!view)
	{
		LOG_ERROR("GraphController has no associated GraphView");
		return;
	}

	std::shared_ptr<Graph> graph = m_graphAccess->getGraphForActiveTokenIds(tokenIds);

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

	view->rebuildGraph(graph, tokenIds, dummyNodes, dummyEdges);
}

DummyNode GraphController::createDummyNodeTopDown(Node* node, std::vector<DummyEdge>* dummyEdges) const
{
	DummyNode result(node);

	node->forEachChildNode(
		[&result, dummyEdges, this](Node* child)
		{
			DummyNode* container = nullptr;

			Edge* edge = child->getMemberEdge();
			TokenComponentAccess* access = edge->getComponent<TokenComponentAccess>();
			if (access)
			{
				TokenComponentAccess::AccessType accessType = access->getAccess();

				for (DummyNode& dummy : result.subNodes)
				{
					if (dummy.accessType == accessType)
					{
						container = &dummy;
						break;
					}
				}

				if (!container)
				{
					result.subNodes.push_back(DummyNode(accessType));
					container = &result.subNodes.back();
				}

			}
			else
			{
				container = &result;
			}

			container->subNodes.push_back(createDummyNodeTopDown(child, dummyEdges));
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
				if (dummy.data->getId() == edge->getId())
				{
					return;
				}
			}

			dummyEdges->push_back(DummyEdge(edge->getFrom()->getId(), edge->getTo()->getId(), edge));
		}
	);

	return result;
}
