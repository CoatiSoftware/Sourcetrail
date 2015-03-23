#include "component/controller/GraphController.h"

#include <set>

#include "utility/logging/logging.h"

#include "component/view/graphElements/GraphEdge.h"
#include "component/view/graphElements/GraphNode.h"
#include "component/view/GraphView.h"
#include "component/view/GraphViewStyle.h"
#include "data/access/StorageAccess.h"
#include "data/graph/Graph.h"

GraphController::GraphController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
{
}

GraphController::~GraphController()
{
}

void GraphController::handleMessage(MessageActivateTokens* message)
{
	if (message->isEdge && message->tokenIds.size() == 1)
	{
		m_currentActiveTokenIds = m_activeTokenIds;
		m_currentActiveTokenIds.push_back(message->tokenIds[0]);

		setActiveAndVisibility(m_currentActiveTokenIds);
		getView()->rebuildGraph(nullptr, m_dummyNodes, m_dummyEdges);
		return;
	}

	if (message->isAggregation)
	{
		m_activeTokenIds.clear();
		m_currentActiveTokenIds = message->tokenIds;

		createDummyGraphForTokenIds(m_currentActiveTokenIds);
		return;
	}

	setActiveTokenIds(message->tokenIds);
	createDummyGraphForTokenIds(m_activeTokenIds);
}

void GraphController::handleMessage(MessageFinishedParsing* message)
{
	getView()->clear();
}

void GraphController::handleMessage(MessageGraphNodeExpand* message)
{
	DummyNode* node = findDummyNodeAccessRecursive(m_dummyNodes, message->tokenId, message->access);
	if (node)
	{
		if (node->autoExpanded)
		{
			node->autoExpanded = false;
			node->expanded = false;
		}
		else
		{
			node->expanded = !node->expanded;
		}

		setActiveAndVisibility(m_currentActiveTokenIds);
		layoutNesting();

		getView()->rebuildGraph(nullptr, m_dummyNodes, m_dummyEdges);
	}
}

void GraphController::handleMessage(MessageGraphNodeMove* message)
{
	DummyNode* node = findDummyNodeRecursive(m_dummyNodes, message->tokenId);
	if (node)
	{
		node->position = message->position;
	}
}

GraphView* GraphController::getView() const
{
	return Controller::getView<GraphView>();
}

void GraphController::setActiveTokenIds(const std::vector<Id>& activeTokenIds)
{
	m_activeTokenIds = activeTokenIds;
	m_currentActiveTokenIds = activeTokenIds;
}

void GraphController::createDummyGraphForTokenIds(const std::vector<Id>& tokenIds)
{
	GraphView* view = getView();
	if (!view)
	{
		LOG_ERROR("GraphController has no associated GraphView");
		return;
	}

	std::shared_ptr<Graph> graph = m_storageAccess->getGraphForActiveTokenIds(tokenIds);

	m_dummyEdges.clear();

	std::set<Id> addedNodes;
	std::vector<DummyNode> dummyNodes;

	graph->forEachNode(
		[&addedNodes, &dummyNodes, this](Node* node)
		{
			Node* parent = node->getLastParentNode();
			Id id = parent->getId();
			if (addedNodes.find(id) != addedNodes.end())
			{
				return;
			}
			addedNodes.insert(id);

			dummyNodes.push_back(createDummyNodeTopDown(parent));
		}
	);

	m_dummyNodes = dummyNodes;

	autoExpandActiveNode(tokenIds);
	setActiveAndVisibility(tokenIds);

	layoutNesting();
	GraphLayouter::layoutSpectralPrototype(m_dummyNodes, m_dummyEdges);

	view->rebuildGraph(graph, m_dummyNodes, m_dummyEdges);
}

DummyNode GraphController::createDummyNodeTopDown(Node* node)
{
	DummyNode result;
	result.data = node;
	result.tokenId = node->getId();

	// there is a global root node with id 0 afaik, so here we actually want the one node below this global root
	Node* parent = node;
	while(parent != NULL && parent->getParentNode() != NULL)
	{
		parent = parent->getParentNode();
	}

	if(parent != NULL)
	{
		result.topLevelAncestorId = parent->getId();
	}

	node->forEachChildNode(
		[node, &result, this](Node* child)
		{
			DummyNode* parent = nullptr;

			Edge* edge = child->getMemberEdge();
			TokenComponentAccess* access = edge->getComponent<TokenComponentAccess>();
			TokenComponentAccess::AccessType accessType = TokenComponentAccess::ACCESS_NONE;

			if (access)
			{
				accessType = access->getAccess();
			}
			else
			{
				if (node->isType(Node::NODE_CLASS | Node::NODE_STRUCT))
				{
					accessType = TokenComponentAccess::ACCESS_PUBLIC;
				}
				else
				{
					parent = &result;
				}
			}

			if (accessType != TokenComponentAccess::ACCESS_NONE)
			{
				for (DummyNode& dummy : result.subNodes)
				{
					if (dummy.accessType == accessType)
					{
						parent = &dummy;
						break;
					}
				}

				if (!parent)
				{
					DummyNode accessNode;
					accessNode.accessType = accessType;
					result.subNodes.push_back(accessNode);
					parent = &result.subNodes.back();

					DummyNode* oldParent = findDummyNodeAccessRecursive(m_dummyNodes, node->getId(), accessType);
					if (oldParent)
					{
						parent->expanded = oldParent->expanded;
					}
				}
			}

			parent->subNodes.push_back(createDummyNodeTopDown(child));
		}
	);

	node->forEachEdge(
		[&result, node, this](Edge* edge)
		{
			if (edge->isType(Edge::EDGE_MEMBER))
			{
				return;
			}

			for (const DummyEdge& dummy : m_dummyEdges)
			{
				if (dummy.data->getId() == edge->getId())
				{
					return;
				}
			}

			m_dummyEdges.push_back(DummyEdge(edge->getFrom()->getId(), edge->getTo()->getId(), edge));
		}
	);

	return result;
}

void GraphController::autoExpandActiveNode(const std::vector<Id>& activeTokenIds)
{
	DummyNode* node = nullptr;
	if (activeTokenIds.size() == 1)
	{
		node = findDummyNodeRecursive(m_dummyNodes, activeTokenIds[0]);
	}

	if (!node)
	{
		return;
	}

	if (node->data->isType(Node::NODE_CLASS | Node::NODE_STRUCT))
	{
		for (DummyNode& subNode : node->subNodes)
		{
			subNode.autoExpanded = true;
		}
	}
}

void GraphController::setActiveAndVisibility(const std::vector<Id>& activeTokenIds)
{
	for (DummyNode& node : m_dummyNodes)
	{
		setNodeActiveRecursive(node, activeTokenIds);
	}

	for (DummyEdge& edge : m_dummyEdges)
	{
		edge.active = false;
		if (find(activeTokenIds.begin(), activeTokenIds.end(), edge.data->getId()) != activeTokenIds.end())
		{
			edge.active = true;
		}

		DummyNode* from = findDummyNodeRecursive(m_dummyNodes, edge.ownerId);
		DummyNode* to = findDummyNodeRecursive(m_dummyNodes, edge.targetId);

		if (from && to && (from->active || to->active || edge.active))
		{
			edge.visible = true;

			if (edge.data->isType(Edge::EDGE_AGGREGATION))
			{
				from->aggregated = true;
				to->aggregated = true;
			}
			else
			{
				from->connected = true;
				to->connected = true;
			}
		}
	}

	for (DummyNode& node : m_dummyNodes)
	{
		setNodeVisibilityRecursiveBottomUp(node, false);
	}
}

void GraphController::setNodeActiveRecursive(DummyNode& node, const std::vector<Id>& activeTokenIds) const
{
	node.visible = false;
	node.active = false;

	if (node.data)
	{
		node.active = find(activeTokenIds.begin(), activeTokenIds.end(), node.data->getId()) != activeTokenIds.end();
	}

	for (DummyNode& subNode : node.subNodes)
	{
		setNodeActiveRecursive(subNode, activeTokenIds);
	}
}

bool GraphController::setNodeVisibilityRecursiveBottomUp(DummyNode& node, bool aggregated) const
{
	bool childVisible = false;
	for (DummyNode& subNode : node.subNodes)
	{
		if (setNodeVisibilityRecursiveBottomUp(subNode, aggregated | node.aggregated))
		{
			childVisible = true;
		}
	}

	if (node.active || node.connected || childVisible || (!aggregated && node.aggregated))
	{
		setNodeVisibilityRecursiveTopDown(node);
	}

	return node.visible;
}

void GraphController::setNodeVisibilityRecursiveTopDown(DummyNode& node) const
{
	node.visible = true;

	for (DummyNode& subNode : node.subNodes)
	{
		if (subNode.accessType != TokenComponentAccess::ACCESS_NONE || node.isExpanded() ||
			(node.data && node.data->isType(Node::NODE_ENUM)) ||
			(node.active && node.data && node.data->isType(Node::NODE_NAMESPACE | Node::NODE_UNDEFINED)))
		{
			setNodeVisibilityRecursiveTopDown(subNode);
		}
	}
}

void GraphController::layoutNesting()
{
	for (DummyNode& node : m_dummyNodes)
	{
		layoutNestingRecursive(node);
	}
}

void GraphController::layoutNestingRecursive(DummyNode& node) const
{
	GraphViewStyle::NodeMargins margins;

	if (node.data)
	{
		margins = GraphViewStyle::getMarginsForNodeType(node.data->getType(), node.subNodes.size() > 0);
	}
	else
	{
		node.invisibleSubNodeCount = 0;
		for (const DummyNode& subNode : node.subNodes)
		{
			if (!subNode.visible)
			{
				node.invisibleSubNodeCount++;
			}
		}

		margins = GraphViewStyle::getMarginsOfAccessNode(
			node.isExpanded(), node.subNodes.size(), node.invisibleSubNodeCount);
	}

	int y = 0;
	int x = 0;
	int width = margins.minWidth;
	int height = 0;

	if (node.data)
	{
		width = margins.charWidth * node.data->getName().size();
	}

	bool layoutHorizontal = true;
	for (DummyNode& subNode : node.subNodes)
	{
		if (!subNode.visible)
		{
			continue;
		}

		layoutNestingRecursive(subNode);

		if (subNode.data || subNode.isExpanded() || subNode.invisibleSubNodeCount != subNode.subNodes.size())
		{
			layoutHorizontal = false;
		}
	}

	for (DummyNode& subNode : node.subNodes)
	{
		if (!subNode.visible)
		{
			continue;
		}

		subNode.position.x = margins.left + x;
		subNode.position.y = margins.top + y;

		if (layoutHorizontal)
		{
			x += subNode.size.x + margins.spacingX;
			if (subNode.size.y > height)
			{
				height = subNode.size.y;
			}
		}
		else
		{
			y += subNode.size.y + margins.spacingY;
			if (subNode.size.x > width)
			{
				width = subNode.size.x;
			}
		}
	}

	if (x > 0)
	{
		x -= margins.spacingX;
	}
	if (y > 0)
	{
		y -= margins.spacingY;
	}

	if (layoutHorizontal && x > width)
	{
		width = x;
	}

	node.size.x = margins.left + width + margins.right;
	node.size.y = margins.top + y + height + margins.bottom;
}

DummyNode* GraphController::findDummyNodeRecursive(std::vector<DummyNode>& nodes, Id tokenId)
{
	for (DummyNode& node : nodes)
	{
		if (node.data && node.data->getId() == tokenId)
		{
			return &node;
		}

		DummyNode* result = findDummyNodeRecursive(node.subNodes, tokenId);
		if (result != nullptr)
		{
			return result;
		}
	}

	return nullptr;
}

DummyNode* GraphController::findDummyNodeAccessRecursive(
	std::vector<DummyNode>& nodes, Id parentId, TokenComponentAccess::AccessType type
){
	DummyNode* node = findDummyNodeRecursive(nodes, parentId);
	if (node)
	{
		for (DummyNode& subNode : node->subNodes)
		{
			if (subNode.accessType == type)
			{
				return &subNode;
			}
		}
	}
	return nullptr;
}
