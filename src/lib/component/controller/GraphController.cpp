#include "component/controller/GraphController.h"

#include "component/view/graphElements/GraphEdge.h"
#include "component/view/graphElements/GraphNode.h"
#include "component/view/GraphView.h"
#include "data/access/GraphAccess.h"

#include "utility/logging/logging.h"

GraphController::GraphController(GraphAccess* graphAccess)
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
		std::vector<std::pair<Id, Id> > rawEdges = m_graphAccess->getConnectedEdges(message->tokenId);
		std::vector<Id> neighbourIds = m_graphAccess->getIdsOfNeighbours(message->tokenId);

		DummyNode node(name, message->tokenId, Vec2i(0,0));

		std::vector<DummyNode> nodes;
		nodes.push_back(node);

		std::vector<DummyEdge> edges;

		int offset = 20; // temporary

		for(unsigned int i = 0; i < rawEdges.size(); i++)
		{
			edges.push_back(DummyEdge(rawEdges[i].first, rawEdges[i].second));

			Id newId = 0;

			if (rawEdges[i].first != message->tokenId)
			{
				name = m_graphAccess->getNameForNodeWithId(rawEdges[i].first);
				newId = rawEdges[i].first;
			}
			else
			{
				name = m_graphAccess->getNameForNodeWithId(rawEdges[i].second);
				newId = rawEdges[i].second;
			}

			nodes.push_back(DummyNode(name, newId, Vec2i(0, offset)));
			offset += 20;
		}

		// find edges between neighbour nodes
		for(unsigned int i = 1; i < nodes.size(); i++)
		{
			std::vector<std::pair<Id, Id>> newEdges = m_graphAccess->getConnectedEdges(nodes[i].tokenId);
			for(unsigned int j = 0; j < newEdges.size(); j++)
			{
				if (std::find(neighbourIds.begin(), neighbourIds.end(), newEdges[j].first) != neighbourIds.end() &&
					std::find(neighbourIds.begin(), neighbourIds.end(), newEdges[j].second) != neighbourIds.end())
				{
					DummyEdge newEdge(newEdges[j].first, newEdges[j].second);

					if (std::find(edges.begin(), edges.end(), newEdge) == edges.end())
					{
						edges.push_back(newEdge);
					}
				}
			}
		}

		view->rebuildGraph(nodes, edges);
	}
}

GraphView* GraphController::getView()
{
	return Controller::getView<GraphView>();
}
