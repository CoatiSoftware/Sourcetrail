#include "component/controller/GraphController.h"

#include <unordered_set>

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
	createDummyGraph(message->tokenId, &GraphLayouter::layoutSimpleRing);
}

GraphView* GraphController::getView()
{
	return Controller::getView<GraphView>();
}

void GraphController::createDummyGraph(const Id activeId, const LayoutFunction layoutFunction)
{
	GraphView* view = getView();
	if (view != NULL)
	{
		std::vector<DummyNode> nodes;

		if (m_graphAccess->checkTokenIsNode(activeId))
		{
			nodes.push_back(createDummyNode(activeId));
			std::vector<DummyNode> neighbours = createNeighbourNodes(nodes[0]);
			nodes.insert(nodes.end(), neighbours.begin(), neighbours.end());
		}
		else
		{
			std::pair<Id, Id> nodeIds = m_graphAccess->getNodesOfEdge(activeId);
			nodes.push_back(createDummyNode(nodeIds.first));
			nodes.push_back(createDummyNode(nodeIds.second));
		}

		layoutFunction(nodes);

		std::vector<DummyEdge> edges;
		edges = createEdges(nodes);

		view->rebuildGraph(nodes, edges);
	}
}

DummyNode GraphController::createDummyNode(const Id nodeId)
{
	DummyNode result("invalid", 0, Vec2i());

	Id topLevelId = findTopLevelNode(nodeId);

	result = buildNodeTopDown(topLevelId);

	return result;
}

Id GraphController::findTopLevelNode(const Id nodeId)
{
	std::vector<std::tuple<Id, Id, Id>> memberEdges = m_graphAccess->getMemberEdgesOfNode(nodeId);

	for (unsigned int i = 0; i < memberEdges.size(); i++)
	{
		if (std::get<0>(memberEdges[i]) != nodeId)
		{
			return findTopLevelNode(std::get<0>(memberEdges[i]));
		}
	}

	return nodeId;
}

DummyNode GraphController::buildNodeTopDown(const Id nodeId)
{
	DummyNode result("invalid", 0, Vec2i());

	result.name = m_graphAccess->getNameForNodeWithId(nodeId);
	result.tokenId = nodeId;

	std::vector<std::tuple<Id, Id, Id>> memberEdges = m_graphAccess->getMemberEdgesOfNode(nodeId);
	for (unsigned int i = 0; i < memberEdges.size(); i++)
	{
		if (std::get<1>(memberEdges[i]) != nodeId)
		{
			result.subNodes.push_back(buildNodeTopDown(std::get<1>(memberEdges[i])));
		}
	}

	return result;
}

std::vector<DummyNode> GraphController::createNeighbourNodes(const Id nodeId)
{
	std::vector<DummyNode> result;

	std::vector<std::tuple<Id, Id, Id>> edges;
	std::vector<std::tuple<Id, Id, Id>> callEdges = m_graphAccess->getCallEdgesOfNode(nodeId);
	std::vector<std::tuple<Id, Id, Id>> usageEdges = m_graphAccess->getUsageEdgesOfNode(nodeId);
	std::vector<std::tuple<Id, Id, Id>> typeOfEdges = m_graphAccess->getTypeOfEdgesOfNode(nodeId);
	std::vector<std::tuple<Id, Id, Id>> returnTypeEdges = m_graphAccess->getReturnTypeOfEdgesOfNode(nodeId);
	std::vector<std::tuple<Id, Id, Id>> parameterEdges = m_graphAccess->getParameterOfEdgesOfNode(nodeId);
	std::vector<std::tuple<Id, Id, Id>> inheritanceEdges = m_graphAccess->getInheritanceEdgesOfNode(nodeId);

	if (callEdges.size() > 0)
	{
		edges.insert(edges.end(), callEdges.begin(), callEdges.end());
	}
	if (usageEdges.size() > 0)
	{
		edges.insert(edges.end(), usageEdges.begin(), usageEdges.end());
	}
	if (typeOfEdges.size() > 0)
	{
		edges.insert(edges.end(), typeOfEdges.begin(), typeOfEdges.end());
	}
	if (returnTypeEdges.size() > 0)
	{
		edges.insert(edges.end(), returnTypeEdges.begin(), returnTypeEdges.end());
	}
	if (parameterEdges.size() > 0)
	{
		edges.insert(edges.end(), parameterEdges.begin(), parameterEdges.end());
	}
	if (inheritanceEdges.size() > 0)
	{
		edges.insert(edges.end(), inheritanceEdges.begin(), inheritanceEdges.end());
	}

	for (unsigned int i = 0; i < edges.size(); i++)
	{
		if (std::get<0>(edges[i]) == nodeId)
		{
			result.push_back(createDummyNode(std::get<1>(edges[i])));
		}
		else
		{
			result.push_back(createDummyNode(std::get<0>(edges[i])));
		}
	}

	return result;
}

std::vector<DummyNode> GraphController::createNeighbourNodes(const DummyNode& node)
{
	std::vector<DummyNode> result;

	std::set<DummyNode> tmpNodes; // to make it easier to keep the nodes unique

	result = createNeighbourNodes(node.tokenId);
	for (unsigned int i = 0; i < result.size(); i++)
	{
		tmpNodes.insert(result[i]);
	}

	for (unsigned int i = 0; i < node.subNodes.size(); i++)
	{
		std::vector<DummyNode> tmpNeighbours = createNeighbourNodes(node.subNodes[i].tokenId);
		for (unsigned int j = 0; j < tmpNeighbours.size(); j++)
		{
			tmpNodes.insert(tmpNeighbours[j]);
		}
	}

	result.clear();

	for (std::set<DummyNode>::iterator it = tmpNodes.begin(); it != tmpNodes.end(); it++)
	{
		result.push_back(*it);
	}

	return result;
}

std::vector<DummyEdge> GraphController::createEdges(const std::vector<DummyNode>& nodes)
{
	std::vector<DummyEdge> result;

	std::unordered_set<Id> nodeIds; // to help discard edges that point to non-existing nodes in the sub-graph
	std::queue<DummyNode> nodeQueue;
	for (unsigned int i = 0; i < nodes.size(); i++)
	{
		nodeQueue.push(nodes[i]);
	}

	while (nodeQueue.size() > 0)
	{
		DummyNode n = nodeQueue.front();
		nodeQueue.pop();

		for (unsigned int i = 0; i < n.subNodes.size(); i++)
		{
			nodeQueue.push(n.subNodes[i]);
		}

		nodeIds.insert(n.tokenId);
	}

	std::set<DummyEdge> tmpEdges; // to make it easier to keep the edges unique
	for (unsigned int i = 0; i < nodes.size(); i++)
	{
		std::set<DummyEdge> tmp = getNeighbourEdgesOfNode(nodes[i]);

		for (std::set<DummyEdge>::iterator it = tmp.begin(); it != tmp.end(); it++)
		{
			if (nodeIds.find(it->ownerId) != nodeIds.end() && nodeIds.find(it->targetId) != nodeIds.end())
			{
				tmpEdges.insert(tmp.begin(), tmp.end());
			}
		}
	}

	for (std::set<DummyEdge>::iterator it = tmpEdges.begin(); it != tmpEdges.end(); it++)
	{
		result.push_back(*it);
	}

	return result;
}

std::set<DummyEdge> GraphController::getNeighbourEdgesOfNode(const DummyNode& node)
{
	std::set<DummyEdge> result;

	std::vector<std::tuple<Id, Id, Id>> callEdges = m_graphAccess->getCallEdgesOfNode(node.tokenId);
	std::vector<std::tuple<Id, Id, Id>> usageEdges = m_graphAccess->getUsageEdgesOfNode(node.tokenId);
	std::vector<std::tuple<Id, Id, Id>> typeOfEdges = m_graphAccess->getTypeOfEdgesOfNode(node.tokenId);
	std::vector<std::tuple<Id, Id, Id>> returnTypeEdges = m_graphAccess->getReturnTypeOfEdgesOfNode(node.tokenId);
	std::vector<std::tuple<Id, Id, Id>> parameterEdges = m_graphAccess->getParameterOfEdgesOfNode(node.tokenId);
	std::vector<std::tuple<Id, Id, Id>> inheritanceEdges = m_graphAccess->getInheritanceEdgesOfNode(node.tokenId);

	for (unsigned int i = 0; i < callEdges.size(); i++)
	{
		result.insert(DummyEdge(std::get<0>(callEdges[i]), std::get<1>(callEdges[i]), std::get<2>(callEdges[i]), Edge::EDGE_CALL));
	}
	for (unsigned int i = 0; i < usageEdges.size(); i++)
	{
		result.insert(DummyEdge(std::get<0>(usageEdges[i]), std::get<1>(usageEdges[i]), std::get<2>(usageEdges[i]), Edge::EDGE_USAGE));
	}
	for (unsigned int i = 0; i < typeOfEdges.size(); i++)
	{
		result.insert(DummyEdge(std::get<0>(typeOfEdges[i]), std::get<1>(typeOfEdges[i]), std::get<2>(typeOfEdges[i]), Edge::EDGE_TYPE_OF));
	}
	for (unsigned int i = 0; i < returnTypeEdges.size(); i++)
	{
		result.insert(DummyEdge(std::get<0>(returnTypeEdges[i]), std::get<1>(returnTypeEdges[i]), std::get<2>(returnTypeEdges[i]), Edge::EDGE_RETURN_TYPE_OF));
	}
	for (unsigned int i = 0; i < parameterEdges.size(); i++)
	{
		result.insert(DummyEdge(std::get<0>(parameterEdges[i]), std::get<1>(parameterEdges[i]), std::get<2>(parameterEdges[i]), Edge::EDGE_PARAMETER_TYPE_OF));
	}
	for (unsigned int i = 0; i < inheritanceEdges.size(); i++)
	{
		result.insert(DummyEdge(std::get<0>(inheritanceEdges[i]), std::get<1>(inheritanceEdges[i]), std::get<2>(inheritanceEdges[i]), Edge::EDGE_INHERITANCE));
	}

	for (unsigned int i = 0; i < node.subNodes.size(); i++)
	{
		std::set<DummyEdge> tmpNeighbours = getNeighbourEdgesOfNode(node.subNodes[i]);
		result.insert(tmpNeighbours.begin(), tmpNeighbours.end());
	}

	return result;
}
