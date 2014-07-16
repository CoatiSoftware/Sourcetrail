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
		std::vector<DummyEdge> edges;

		nodes.push_back(createDummyNode(activeId));

		std::vector<DummyNode> neighbours = createNeighbourNodes(nodes[0]);
		nodes.insert(nodes.end(), neighbours.begin(), neighbours.end());

		layoutFunction(nodes);

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
	std::vector<std::pair<Id, Id>> memberEdges = m_graphAccess->getMemberEdgesOfNode(nodeId);

	for(unsigned int i = 0; i < memberEdges.size(); i++)
	{
		if(memberEdges[i].first != nodeId)
		{
			return findTopLevelNode(memberEdges[i].first);
		}
	}

	return nodeId;
}

DummyNode GraphController::buildNodeTopDown(const Id nodeId)
{
	DummyNode result("invalid", 0, Vec2i());

	result.name = m_graphAccess->getNameForNodeWithId(nodeId);
	result.tokenId = nodeId;

	std::vector<std::pair<Id, Id>> memberEdges = m_graphAccess->getMemberEdgesOfNode(nodeId);

	for(unsigned int i = 0; i < memberEdges.size(); i++)
	{
		if(memberEdges[i].second != nodeId)
		{
			result.subNodes.push_back(buildNodeTopDown(memberEdges[i].second));
		}
	}

	return result;
}

std::vector<DummyNode> GraphController::createNeighbourNodes(const Id nodeId)
{
	std::vector<DummyNode> result;

	std::vector<std::pair<Id, Id>> edges;
	std::vector<std::pair<Id, Id>> callEdges = m_graphAccess->getCallEdgesOfNode(nodeId);
	std::vector<std::pair<Id, Id>> usageEdges = m_graphAccess->getUsageEdgesOfNode(nodeId);
	std::vector<std::pair<Id, Id>> typeOfEdges = m_graphAccess->getTypeOfEdgesOfNode(nodeId);
	std::vector<std::pair<Id, Id>> returnTypeEdges = m_graphAccess->getReturnTypeOfEdgesOfNode(nodeId);
	std::vector<std::pair<Id, Id>> parameterEdges = m_graphAccess->getParameterOfEdgesOfNode(nodeId);

	if(callEdges.size() > 0)
	{
		edges.insert(edges.end(), callEdges.begin(), callEdges.end());
	}
	if(usageEdges.size() > 0)
	{
		edges.insert(edges.end(), usageEdges.begin(), usageEdges.end());
	}
	if(typeOfEdges.size() > 0)
	{
		edges.insert(edges.end(), typeOfEdges.begin(), typeOfEdges.end());
	}
	if(returnTypeEdges.size() > 0)
	{
		edges.insert(edges.end(), returnTypeEdges.begin(), returnTypeEdges.end());
	}
	if(parameterEdges.size() > 0)
	{
		edges.insert(edges.end(), parameterEdges.begin(), parameterEdges.end());
	}

	for(unsigned int i = 0; i < edges.size(); i++)
	{
		if(edges[i].first == nodeId)
		{
			result.push_back(createDummyNode(edges[i].second));
		}
		else
		{
			result.push_back(createDummyNode(edges[i].first));
		}
	}

	return result;
}

std::vector<DummyNode> GraphController::createNeighbourNodes(const DummyNode& node)
{
	std::vector<DummyNode> result;

	std::set<DummyNode> tmpNodes; // to make it easier to keep the nodes unique

	result = createNeighbourNodes(node.tokenId);
	for(unsigned int i = 0; i < result.size(); i++)
	{
		tmpNodes.insert(result[i]);
	}

	for(unsigned int i = 0; i < node.subNodes.size(); i++)
	{
		std::vector<DummyNode> tmpNeighbours = createNeighbourNodes(node.subNodes[i].tokenId);
		for(unsigned int j = 0; j < tmpNeighbours.size(); j++)
		{
			tmpNodes.insert(tmpNeighbours[j]);
		}
	}

	result.clear();
	std::set<DummyNode>::iterator it = tmpNodes.begin();
	for(it; it != tmpNodes.end(); it++)
	{
		result.push_back(*it);
	}

	return result;
}

std::vector<DummyEdge> GraphController::createEdges(const std::vector<DummyNode>& nodes)
{
	std::vector<DummyEdge> result;

	std::unordered_set<Id> nodeIds; // to help discard edges that point to non-existing nodes in the sub-graph
	for(unsigned int i = 0; i < nodes.size(); i++)
	{
		nodeIds.insert(nodes[i].tokenId);
	}

	std::set<DummyEdge> tmpEdges; // to make it easier to keep the edges unique
	for(unsigned int i = 0; i < nodes.size(); i++)
	{
		std::set<DummyEdge> tmp = getNeighbourEdgesOfNode(nodes[i]);

		std::set<DummyEdge>::iterator it = tmp.begin();
		for(it; it != tmp.end(); it++)
		{
			if(nodeIds.find(it->ownerId) != nodeIds.end() && nodeIds.find(it->targetId) != nodeIds.end())
			{
				tmpEdges.insert(tmp.begin(), tmp.end());
			}
		}
	}

	std::set<DummyEdge>::iterator it = tmpEdges.begin();
	for(it; it != tmpEdges.end(); it++)
	{
		result.push_back(*it);
	}

	return result;
}

std::set<DummyEdge> GraphController::getNeighbourEdgesOfNode(const DummyNode& node)
{
	std::set<DummyEdge> result;

	std::vector<std::pair<Id, Id>> callEdges = m_graphAccess->getCallEdgesOfNode(node.tokenId);
	std::vector<std::pair<Id, Id>> usageEdges = m_graphAccess->getUsageEdgesOfNode(node.tokenId);
	std::vector<std::pair<Id, Id>> typeOfEdges = m_graphAccess->getTypeOfEdgesOfNode(node.tokenId);
	std::vector<std::pair<Id, Id>> returnTypeEdges = m_graphAccess->getReturnTypeOfEdgesOfNode(node.tokenId);
	std::vector<std::pair<Id, Id>> parameterEdges = m_graphAccess->getParameterOfEdgesOfNode(node.tokenId);

	for(unsigned int i = 0; i < callEdges.size(); i++)
	{
		result.insert(DummyEdge(callEdges[i].first, callEdges[i].second, Edge::EdgeType::EDGE_CALL));
	}
	for(unsigned int i = 0; i < usageEdges.size(); i++)
	{
		result.insert(DummyEdge(usageEdges[i].first, usageEdges[i].second, Edge::EdgeType::EDGE_USAGE));
	}
	for(unsigned int i = 0; i < typeOfEdges.size(); i++)
	{
		result.insert(DummyEdge(typeOfEdges[i].first, typeOfEdges[i].second, Edge::EdgeType::EDGE_TYPE_OF));
	}
	for(unsigned int i = 0; i < returnTypeEdges.size(); i++)
	{
		result.insert(DummyEdge(returnTypeEdges[i].first, returnTypeEdges[i].second, Edge::EdgeType::EDGE_RETURN_TYPE_OF));
	}
	for(unsigned int i = 0; i < parameterEdges.size(); i++)
	{
		result.insert(DummyEdge(parameterEdges[i].first, parameterEdges[i].second, Edge::EdgeType::EDGE_PARAMETER_TYPE_OF));
	}

	for(unsigned int i = 0; i < node.subNodes.size(); i++)
	{
		std::set<DummyEdge> tmpNeighbours = getNeighbourEdgesOfNode(node.subNodes[i]);
		result.insert(tmpNeighbours.begin(), tmpNeighbours.end());
	}

	return result;
}
