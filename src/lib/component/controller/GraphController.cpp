#include "component/controller/GraphController.h"

#include <set>

#include "utility/logging/logging.h"

#include "component/view/graphElements/GraphEdge.h"
#include "component/view/graphElements/GraphNode.h"
#include "component/view/GraphView.h"
#include "data/access/GraphAccess.h"

GraphController::Margins::Margins()
	: left(0)
	, right(0)
	, top(0)
	, bottom(0)
	, betweenX(0)
	, betweenY(0)
	, minWidth(0)
	, charWidth(0.0f)
{
}


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

void GraphController::handleMessage(MessageGraphNodeExpand* message)
{
	DummyNode* node = findDummyNodeAccessRecursive(m_dummyNodes, message->tokenId, message->access);
	if (node)
	{
		node->expanded = !node->expanded;

		setActiveAndVisibility(m_activeTokenIds);
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

void GraphController::createDummyGraphForTokenIds(const std::vector<Id>& tokenIds)
{
	const GraphLayouter::LayoutFunction layoutFunction = &GraphLayouter::layoutSimpleRaster;

	GraphView* view = getView();
	if (!view)
	{
		LOG_ERROR("GraphController has no associated GraphView");
		return;
	}

	std::shared_ptr<Graph> graph = m_graphAccess->getGraphForActiveTokenIds(tokenIds);


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

	m_activeTokenIds = tokenIds;
	setActiveAndVisibility(tokenIds);

	layoutNesting();
	layoutFunction(m_dummyNodes);

	view->rebuildGraph(graph, m_dummyNodes, m_dummyEdges);
}

DummyNode GraphController::createDummyNodeTopDown(Node* node)
{
	DummyNode result(node);

	node->forEachChildNode(
		[node, &result, this](Node* child)
		{
			DummyNode* parent = nullptr;

			Edge* edge = child->getMemberEdge();
			TokenComponentAccess* access = edge->getComponent<TokenComponentAccess>();
			if (access)
			{
				TokenComponentAccess::AccessType accessType = access->getAccess();

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
					result.subNodes.push_back(DummyNode(accessType));
					parent = &result.subNodes.back();

					DummyNode* oldParent = findDummyNodeAccessRecursive(m_dummyNodes, node->getId(), accessType);
					if (oldParent)
					{
						parent->expanded = oldParent->expanded;
					}
				}
			}
			else
			{
				parent = &result;
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

			if (edge->isType(Edge::EDGE_AGGREGATION))
			{
				result.aggregated = true;
			}
			else
			{
				result.connected = true;
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

void GraphController::setActiveAndVisibility(const std::vector<Id>& activeTokenIds)
{
	for (DummyNode& node : m_dummyNodes)
	{
		setNodeActiveAndVisibilityRecursiveBottomUp(node, activeTokenIds, false);
	}

	for (DummyEdge& edge : m_dummyEdges)
	{
		edge.visible = true;
		if (find(activeTokenIds.begin(), activeTokenIds.end(), edge.data->getId()) != activeTokenIds.end())
		{
			edge.active = true;
		}
	}
}

bool GraphController::setNodeActiveAndVisibilityRecursiveBottomUp(
	DummyNode& node, const std::vector<Id>& activeTokenIds, bool aggregated
) const {
	node.visible = false;
	node.active = false;

	if (node.data)
	{
		node.active = find(activeTokenIds.begin(), activeTokenIds.end(), node.data->getId()) != activeTokenIds.end();
	}

	bool childVisible = false;
	for (DummyNode& subNode : node.subNodes)
	{
		if (setNodeActiveAndVisibilityRecursiveBottomUp(subNode, activeTokenIds, aggregated | node.aggregated))
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
		if (subNode.accessType != TokenComponentAccess::ACCESS_NONE || node.expanded ||
			(node.data && node.data->getType() == Node::NODE_ENUM))
		{
			setNodeVisibilityRecursiveTopDown(subNode);
		}
	}
}

void GraphController::layoutNesting()
{
	if (!m_viewMetrics.width)
	{
		m_viewMetrics = getView()->getViewMetrics();
	}

	for (DummyNode& node : m_dummyNodes)
	{
		layoutNestingRecursive(node);
	}
}

void GraphController::layoutNestingRecursive(DummyNode& node) const
{
	Margins margins = getMarginsForDummyNode(node);

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

		if (subNode.data || subNode.expanded || subNode.invisibleSubNodeCount != subNode.subNodes.size())
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
			x += subNode.size.x + margins.betweenX;
			if (subNode.size.y > height)
			{
				height = subNode.size.y;
			}
		}
		else
		{
			y += subNode.size.y + margins.betweenY;
			if (subNode.size.x > width)
			{
				width = subNode.size.x;
			}
		}
	}

	if (x > 0)
	{
		x -= margins.betweenX;
	}
	if (y > 0)
	{
		y -= margins.betweenY;
	}

	if (layoutHorizontal && x > width)
	{
		width = x;
	}

	node.size.x = margins.left + width + margins.right;
	node.size.y = margins.top + y + height + margins.bottom;
}

GraphController::Margins GraphController::getMarginsForDummyNode(DummyNode& node) const
{
	Margins margins;
	margins.betweenX = margins.betweenY = 8;

	if (node.data)
	{
		switch (node.data->getType())
		{
		case Node::NODE_UNDEFINED:
		case Node::NODE_NAMESPACE:
			margins.left = margins.right = 15;
			margins.top = 28;
			margins.bottom = 15;

			margins.charWidth = m_viewMetrics.typeNameCharWidth;
			break;

		case Node::NODE_UNDEFINED_TYPE:
		case Node::NODE_STRUCT:
		case Node::NODE_CLASS:
		case Node::NODE_ENUM:
		case Node::NODE_TYPEDEF:
		case Node::NODE_TEMPLATE_PARAMETER_TYPE:
			if (node.subNodes.size())
			{
				margins.left = margins.right = 15;
				margins.top = 30;
				margins.bottom = 10;
			}
			else
			{
				margins.left = margins.right = 8;
				margins.top = margins.bottom = 13;
			}

			margins.charWidth = m_viewMetrics.typeNameCharWidth;
			break;

		case Node::NODE_UNDEFINED_FUNCTION:
		case Node::NODE_FUNCTION:
		case Node::NODE_METHOD:
			margins.left = margins.right = 5;
			margins.top = margins.bottom = 10;

			margins.charWidth = m_viewMetrics.functionNameCharWidth;
			break;

		case Node::NODE_UNDEFINED_VARIABLE:
		case Node::NODE_GLOBAL_VARIABLE:
		case Node::NODE_FIELD:
			margins.left = margins.right = 5;
			margins.top = margins.bottom = 10;

			margins.charWidth = m_viewMetrics.variableNameCharWidth;
			break;
		}
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

		margins.left = margins.right = 10;
		margins.top = 40;

		if (node.invisibleSubNodeCount == node.subNodes.size())
		{
			margins.minWidth = 20;
			margins.bottom = 10;
		}
		else
		{
			margins.minWidth = 82;

			if (node.expanded)
			{
				margins.bottom = 15;
			}
			else if (node.invisibleSubNodeCount)
			{
				margins.bottom = 23;
			}
			else
			{
				margins.bottom = 10;
			}
		}
	}

	return margins;
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
