#include "component/controller/GraphController.h"

#include <set>

#include "utility/logging/logging.h"
#include "utility/utility.h"

#include "component/controller/helper/DummyEdge.h"
#include "component/controller/helper/DummyNode.h"
#include "component/controller/helper/GraphLayouter.h"
#include "component/controller/helper/GraphPostprocessor.h"
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
	if (message->isEdge || message->keepContent())
	{
		m_activeEdgeIds = message->tokenIds;
		setActiveAndVisibility(utility::concat(m_activeNodeIds, m_activeEdgeIds));
		buildGraph(message);
		return;
	}
	else if (message->isAggregation)
	{
		m_activeNodeIds.clear();
		m_activeEdgeIds = message->tokenIds;
	}
	else
	{
		m_activeNodeIds = message->tokenIds;
		m_activeEdgeIds.clear();
	}

	if (!m_activeNodeIds.size() && !m_activeEdgeIds.size())
	{
		clear();
		return;
	}

	createDummyGraphForTokenIds(utility::concat(m_activeNodeIds, m_activeEdgeIds));

	buildGraph(message);
}

void GraphController::handleMessage(MessageFlushUpdates* message)
{
	buildGraph(message);
}

void GraphController::handleMessage(MessageFocusIn* message)
{
	getView()->focusTokenIds(message->tokenIds);
}

void GraphController::handleMessage(MessageFocusOut *message)
{
	getView()->defocusTokenIds(message->tokenIds);
}

void GraphController::handleMessage(MessageGraphNodeBundleSplit* message)
{
	for (size_t i = 0; i < m_dummyNodes.size(); i++)
	{
		DummyNode& node = m_dummyNodes[i];
		if (node.isBundleNode() && node.tokenId == message->bundleId)
		{
			m_dummyNodes.insert(m_dummyNodes.end(), node.bundledNodes.begin(), node.bundledNodes.end());
			m_dummyNodes.erase(m_dummyNodes.begin() + i);
			break;
		}
	}

	for (size_t i = 0; i < m_dummyEdges.size(); i++)
	{
		DummyEdge& edge = m_dummyEdges[i];
		if (!edge.data && edge.targetId == message->bundleId)
		{
			m_dummyEdges.erase(m_dummyEdges.begin() + i);
			break;
		}
	}

	setActiveAndVisibility(utility::concat(m_activeNodeIds, m_activeEdgeIds));

	layoutNesting();
	layoutGraph();

	buildGraph(message);
}

void GraphController::handleMessage(MessageGraphNodeExpand* message)
{
	DummyNode* node = findDummyNodeRecursive(m_dummyNodes, message->tokenId);
	if (node)
	{
		node->expanded = message->expand;

		setActiveAndVisibility(utility::concat(m_activeNodeIds, m_activeEdgeIds));

		layoutNesting();
		layoutGraph();

		buildGraph(message);
	}
}

void GraphController::handleMessage(MessageGraphNodeMove* message)
{
	DummyNode* node = findDummyNodeRecursive(m_dummyNodes, message->tokenId);
	if (node)
	{
		node->position += message->delta;

		if (message->isFresh())
		{
			getView()->resizeView();
		}
		else
		{
			buildGraph(message);
		}
	}
}

void GraphController::handleMessage(MessageShowErrors* message)
{
	clear();
}

GraphView* GraphController::getView() const
{
	return Controller::getView<GraphView>();
}

void GraphController::clear()
{
	m_dummyNodes.clear();
	m_dummyEdges.clear();

	m_activeNodeIds.clear();
	m_activeEdgeIds.clear();

	m_graph.reset();
	getView()->clear();
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

	for (DummyNode& node : dummyNodes)
	{
		node.hasParent = false;
	}

	m_dummyNodes = dummyNodes;

	autoExpandActiveNode(tokenIds);
	setActiveAndVisibility(tokenIds);

	bundleNodes();

	layoutNesting();
	layoutGraph();

	m_graph = graph;
}

DummyNode GraphController::createDummyNodeTopDown(Node* node)
{
	DummyNode result;
	result.data = node;
	result.tokenId = node->getId();

	// there is a global root node with id 0 afaik, so here we actually want the one node below this global root
	Node* parent = node;
	while (parent != NULL && parent->getParentNode() != NULL)
	{
		parent = parent->getParentNode();
	}

	if (parent != NULL)
	{
		result.topLevelAncestorId = parent->getId();
	}

	// Expand nodes that were expanded before.
	DummyNode* oldNode = findDummyNodeRecursive(m_dummyNodes, node->getId());
	if (oldNode)
	{
		result.expanded = oldNode->isExpanded();
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
				if (node->isType(Node::NODE_TYPE | Node::NODE_CLASS | Node::NODE_STRUCT))
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
				}
			}

			parent->subNodes.push_back(createDummyNodeTopDown(child));
		}
	);

	node->forEachEdgeOfType(
		~Edge::EDGE_MEMBER,
		[&result, node, this](Edge* edge)
		{
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

	if (node)
	{
		node->expanded = true;
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
		if (!edge.data)
		{
			continue;
		}

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
			from->connected = true;
			to->connected = true;
		}
	}

	for (DummyNode& node : m_dummyNodes)
	{
		setNodeVisibilityRecursiveBottomUp(node);
	}
}

void GraphController::setNodeActiveRecursive(DummyNode& node, const std::vector<Id>& activeTokenIds) const
{
	node.active = false;

	if (node.isGraphNode())
	{
		node.active = find(activeTokenIds.begin(), activeTokenIds.end(), node.data->getId()) != activeTokenIds.end();
	}

	for (DummyNode& subNode : node.subNodes)
	{
		setNodeActiveRecursive(subNode, activeTokenIds);
	}
}

bool GraphController::setNodeVisibilityRecursiveBottomUp(DummyNode& node) const
{
	node.visible = false;
	node.childVisible = false;

	if (node.isExpandToggleNode())
	{
		node.visible = true;
		return false;
	}
	else if (node.isBundleNode())
	{
		node.visible = true;
		return true;
	}

	for (DummyNode& subNode : node.subNodes)
	{
		if (setNodeVisibilityRecursiveBottomUp(subNode))
		{
			node.childVisible = true;
		}
	}

	if (node.active || node.connected || node.childVisible)
	{
		setNodeVisibilityRecursiveTopDown(node, false);
	}

	return node.visible;
}

void GraphController::setNodeVisibilityRecursiveTopDown(DummyNode& node, bool parentExpanded) const
{
	node.visible = true;

	if ((node.isGraphNode() && node.isExpanded()) ||
		(node.isAccessNode() && parentExpanded) ||
		(node.isGraphNode() && parentExpanded && node.data->isType(Node::NODE_ENUM_CONSTANT)))
	{
		for (DummyNode& subNode : node.subNodes)
		{
			node.childVisible = true;
			setNodeVisibilityRecursiveTopDown(subNode, node.isExpanded());
		}
	}
}

void GraphController::bundleNodes()
{
	bundleNodesMatching(
		[&](const DummyNode& node)
		{
			return isTypeNodeWithSingleAggregation(node, TokenComponentAggregation::DIRECTION_BACKWARD);
		},
		3,
		"Referenced Types"
	);

	bundleNodesMatching(
		[&](const DummyNode& node)
		{
			return isTypeNodeWithSingleAggregation(node, TokenComponentAggregation::DIRECTION_FORWARD);
		},
		3,
		"Referencing Types"
	);

	bundleNodesMatching(
		[&](const DummyNode& node)
		{
			return isTypeNodeWithSingleInheritance(node, true);
		},
		3,
		"Base Types"
	);

	bundleNodesMatching(
		[&](const DummyNode& node)
		{
			return isTypeNodeWithSingleInheritance(node, false);
		},
		3,
		"Derived Types"
	);

	bundleNodesMatching(
		[](const DummyNode& node)
		{
			if (node.visible && node.isGraphNode() && !node.hasActiveSubNode() && !node.data->isDefined())
			{
				return true;
			}

			return false;
		},
		1,
		"Undefined Symbols"
	);
}

void GraphController::bundleNodesMatching(std::function<bool(const DummyNode&)> matcher, size_t count, const std::string& name)
{
	size_t matchCount = 0;
	std::vector<size_t> nodeIndices;

	for (size_t i = 0; i < m_dummyNodes.size(); i++)
	{
		const DummyNode& node = m_dummyNodes[i];

		if (matcher(node))
		{
			matchCount++;
			nodeIndices.push_back(i);
		}
	}

	if (matchCount < count || matchCount == m_dummyNodes.size())
	{
		return;
	}

	DummyNode bundleNode;
	bundleNode.name = name;
	bundleNode.visible = true;

	for (int i = nodeIndices.size() - 1; i >= 0; i--)
	{
		DummyNode& node = m_dummyNodes[nodeIndices[i]];
		node.visible = false;

		bundleNode.bundledNodes.push_back(node);
		if (!bundleNode.tokenId)
		{
			bundleNode.tokenId = node.data->getId();
		}

		m_dummyNodes.erase(m_dummyNodes.begin() + nodeIndices[i]);
	}

	std::vector<DummyEdge> bundleEdges;
	for (DummyNode& node : bundleNode.bundledNodes)
	{
		for (DummyEdge& edge : m_dummyEdges)
		{
			bool owner = (edge.ownerId == node.data->getId());
			bool target = (edge.targetId == node.data->getId());

			if (!owner && !target)
			{
				continue;
			}

			DummyEdge* bundleEdgePtr = nullptr;
			for (DummyEdge& bundleEdge : bundleEdges)
			{
				if ((owner && bundleEdge.ownerId == edge.targetId) ||
					(target && bundleEdge.ownerId == edge.ownerId))
				{
					bundleEdgePtr = &bundleEdge;
					break;
				}
			}

			if (!bundleEdgePtr)
			{
				DummyEdge bundleEdge;
				bundleEdge.visible = true;
				bundleEdge.ownerId = (owner ? edge.targetId : edge.ownerId);
				bundleEdge.targetId = bundleNode.bundledNodes.front().data->getId();
				bundleEdges.push_back(bundleEdge);
				bundleEdgePtr = &bundleEdges.back();
			}

			bundleEdgePtr->weight += edge.getWeight();
			bundleEdgePtr->updateDirection(edge.getDirection(), owner);
			edge.visible = false;
		}
	}

	m_dummyEdges.insert(m_dummyEdges.end(), bundleEdges.begin(), bundleEdges.end());
	m_dummyNodes.push_back(bundleNode);
}

bool GraphController::isTypeNodeWithSingleAggregation(
	const DummyNode& node, TokenComponentAggregation::Direction direction
) const {
	const Node::NodeTypeMask typeMask = Node::NODE_STRUCT | Node::NODE_CLASS;

	if (!node.visible || !node.isGraphNode() || node.hasVisibleSubNode() || !node.data->isType(typeMask))
	{
		return false;
	}

	bool matches = false;
	int count = 0;
	Id tokenId = node.data->getId();

	node.data->forEachEdgeOfType(
		~Edge::EDGE_MEMBER,
		[direction, tokenId, &matches, &count](Edge* edge)
		{
			count++;

			if (edge->isType(Edge::EDGE_AGGREGATION))
			{
				TokenComponentAggregation::Direction dir =
					edge->getComponent<TokenComponentAggregation>()->getDirection();

				if ((edge->getFrom()->getId() == tokenId && dir == direction) ||
					(edge->getTo()->getId() == tokenId && dir == TokenComponentAggregation::opposite(direction)))
				{
					matches = true;
				}
			}
		}
	);

	if (count > 1)
	{
		return false;
	}

	return matches;
}

bool GraphController::isTypeNodeWithSingleInheritance(const DummyNode& node, bool isBase) const
{
	const Node::NodeTypeMask typeMask = Node::NODE_STRUCT | Node::NODE_CLASS;

	if (!node.visible || !node.isGraphNode() || node.hasVisibleSubNode() || !node.data->isType(typeMask))
	{
		return false;
	}

	bool matches = false;
	Id tokenId = node.data->getId();

	node.data->forEachEdgeOfType(
		Edge::EDGE_INHERITANCE,
		[isBase, tokenId, &matches](Edge* edge)
		{
			if ((!isBase && edge->getFrom()->getId() == tokenId) ||
				(isBase && edge->getTo()->getId() == tokenId))
			{
				matches = true;
			}
		}
	);

	return matches;
}

void GraphController::layoutNesting()
{
	for (DummyNode& node : m_dummyNodes)
	{
		layoutNestingRecursive(node);
	}

	for (DummyNode& node : m_dummyNodes)
	{
		layoutToGrid(node);
	}
}

void GraphController::layoutNestingRecursive(DummyNode& node) const
{
	if (!node.visible)
	{
		return;
	}

	GraphViewStyle::NodeMargins margins;

	if (node.isGraphNode())
	{
		margins = GraphViewStyle::getMarginsForNodeType(node.data->getType(), node.childVisible);
	}
	else if (node.isAccessNode())
	{
		margins = GraphViewStyle::getMarginsOfAccessNode(node.accessType);
	}
	else if (node.isExpandToggleNode())
	{
		margins = GraphViewStyle::getMarginsOfExpandToggleNode();
	}
	else if (node.isBundleNode())
	{
		margins = GraphViewStyle::getMarginsOfBundleNode();
	}

	int y = 0;
	int x = 0;
	int width = margins.minWidth;
	int height = 0;

	if (node.isGraphNode())
	{
		if (!node.hasParent)
		{
			width = margins.charWidth * node.data->getFullName().size();
		}
		else
		{
			width = margins.charWidth * node.data->getName().size();
		}

		if (node.data->isType(Node::NODE_TYPE | Node::NODE_CLASS | Node::NODE_STRUCT | Node::NODE_ENUM) && node.subNodes.size())
		{
			addExpandToggleNode(node);
		}
	}
	else if (node.isBundleNode())
	{
		width = margins.charWidth * node.name.size();
	}

	width += margins.iconWidth;

	// Horizontal layouting is currently not used, but left in place for experimentation.
	bool layoutHorizontal = false;

	for (DummyNode& subNode : node.subNodes)
	{
		if (!subNode.visible)
		{
			continue;
		}

		layoutNestingRecursive(subNode);

		if (subNode.isExpandToggleNode())
		{
			width += margins.spacingX + subNode.size.x;
		}
	}

	for (DummyNode& subNode : node.subNodes)
	{
		if (!subNode.visible || subNode.isExpandToggleNode())
		{
			continue;
		}

		subNode.position.x = margins.left + x;
		subNode.position.y = margins.top + margins.charHeight + y;

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
	node.size.y = margins.top + margins.charHeight + y + height + margins.bottom;

	for (DummyNode& subNode : node.subNodes)
	{
		if (!subNode.visible)
		{
			continue;
		}

		if (subNode.isAccessNode())
		{
			subNode.size.x = width;
		}
		else if (subNode.isExpandToggleNode())
		{
			subNode.position.x = margins.left + width - subNode.size.x;
			subNode.position.y = 6;
		}
	}
}

void GraphController::addExpandToggleNode(DummyNode& node) const
{
	DummyNode expandNode;
	expandNode.visible = true;
	expandNode.expanded = node.expanded;

	for (size_t i = 0; i < node.subNodes.size(); i++)
	{
		DummyNode& subNode = node.subNodes[i];

		if (subNode.isExpandToggleNode())
		{
			node.subNodes.erase(node.subNodes.begin() + i);
			i--;
			continue;
		}
		else if (subNode.isGraphNode() && subNode.data->isType(Node::NODE_ENUM_CONSTANT) && !subNode.visible)
		{
			expandNode.invisibleSubNodeCount++;
			continue;
		}

		for (DummyNode& subSubNode : subNode.subNodes)
		{
			if (!subSubNode.visible)
			{
				expandNode.invisibleSubNodeCount++;
			}
		}
	}

	if (expandNode.isExpanded() || expandNode.invisibleSubNodeCount)
	{
		node.subNodes.push_back(expandNode);
	}
}

void GraphController::layoutToGrid(DummyNode& node) const
{
	if (!node.visible)
	{
		return;
	}

	size_t width = GraphViewStyle::toGridSize(node.size.x);
	size_t height = GraphViewStyle::toGridSize(node.size.y);

	size_t incX = width - node.size.x;
	size_t incY = height - node.size.y;

	node.size.x = width;
	node.size.y = height;

	if (!node.isGraphNode())
	{
		return;
	}

	DummyNode* lastAccessNode = nullptr;
	for (DummyNode& subNode : node.subNodes)
	{
		if (!subNode.visible)
		{
			continue;
		}

		if (subNode.isAccessNode())
		{
			subNode.size.x = subNode.size.x + incX;
			lastAccessNode = &subNode;
		}
		else if (subNode.isExpandToggleNode())
		{
			subNode.position.x = subNode.position.x + incX;
		}
	}

	if (lastAccessNode)
	{
		lastAccessNode->size.y = lastAccessNode->size.y + incY;
	}
}

void GraphController::layoutGraph()
{
	// GraphLayouter::layoutSpectralPrototype(m_dummyNodes, m_dummyEdges);
	// GraphPostprocessor::doPostprocessing(m_dummyNodes);

	GraphLayouter::layoutBucket(m_dummyNodes, m_dummyEdges, getView()->getViewSize());
}

DummyNode* GraphController::findDummyNodeRecursive(std::vector<DummyNode>& nodes, Id tokenId) const
{
	for (DummyNode& node : nodes)
	{
		if (node.isGraphNode() && node.data->getId() == tokenId)
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

DummyNode* GraphController::findTopLevelDummyNodeRecursive(std::vector<DummyNode>& nodes, Id tokenId) const
{
	for (DummyNode& node : nodes)
	{
		if (node.isGraphNode() && node.data->getId() == tokenId)
		{
			return &node;
		}
	}

	return nullptr;
}

DummyNode* GraphController::findDummyNodeAccessRecursive(
	std::vector<DummyNode>& nodes, Id parentId, TokenComponentAccess::AccessType type
) const {
	DummyNode* node = findDummyNodeRecursive(nodes, parentId);
	if (node)
	{
		for (DummyNode& subNode : node->subNodes)
		{
			if (subNode.isAccessNode() && subNode.accessType == type)
			{
				return &subNode;
			}
		}
	}
	return nullptr;
}

void GraphController::buildGraph(MessageBase* message)
{
	if (message->undoRedoType == MessageBase::UNDOTYPE_NORMAL)
	{
		getView()->rebuildGraph(m_graph, m_dummyNodes, m_dummyEdges);
		m_graph.reset();
	}
}
