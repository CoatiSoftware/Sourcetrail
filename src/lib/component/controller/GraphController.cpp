#include "component/controller/GraphController.h"

#include <set>

#include "utility/logging/logging.h"
#include "utility/utility.h"
#include "utility/utilityString.h"

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

void GraphController::handleMessage(MessageActivateAll* message)
{
	m_activeNodeIds.clear();
	m_activeEdgeIds.clear();

	createDummyGraphForTokenIds(std::vector<Id>(), m_storageAccess->getGraphForAll());

	bundleNodesByType();

	layoutNesting();
	layoutGraph();

	buildGraph(message);
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

	std::vector<Id> tokenIds = utility::concat(m_activeNodeIds, m_activeEdgeIds);

	std::shared_ptr<Graph> graph = m_storageAccess->getGraphForActiveTokenIds(tokenIds);

	createDummyGraphForTokenIds(tokenIds, graph);

	if (m_activeNodeIds.size() == 1)
	{
		bundleNodes();
	}

	layoutNesting();
	layoutGraph();

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
			m_dummyNodes.insert(m_dummyNodes.begin() + i + 1, node.bundledNodes.begin(), node.bundledNodes.end());
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

		if (message->isReplayed())
		{
			buildGraph(message);
		}
		else
		{
			getView()->resizeView();
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

void GraphController::createDummyGraphForTokenIds(const std::vector<Id>& tokenIds, const std::shared_ptr<Graph> graph)
{
	GraphView* view = getView();
	if (!view)
	{
		LOG_ERROR("GraphController has no associated GraphView");
		return;
	}

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
		node.name = node.data->getFullName();
	}

	m_dummyNodes = dummyNodes;

	autoExpandActiveNode(tokenIds);
	setActiveAndVisibility(tokenIds);

	m_graph = graph;
}

DummyNode GraphController::createDummyNodeTopDown(Node* node)
{
	DummyNode result;
	result.data = node;
	result.tokenId = node->getId();
	result.name = node->getName();

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

	// Expand nodes that were expanded before, except functions.
	DummyNode* oldNode = findDummyNodeRecursive(m_dummyNodes, node->getId());
	if (oldNode && oldNode->isGraphNode() && !oldNode->data->isType(Node::NODE_FUNCTION | Node::NODE_METHOD))
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
	bool noActive = activeTokenIds.size() == 0;
	if (activeTokenIds.size() > 0)
	{
		noActive = true;
		for (DummyNode& node : m_dummyNodes)
		{
			setNodeActiveRecursive(node, activeTokenIds, &noActive);
		}
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

		if (from && to && (noActive || from->active || to->active || edge.active))
		{
			edge.visible = true;
			from->connected = true;
			to->connected = true;
		}
	}

	for (DummyNode& node : m_dummyNodes)
	{
		removeImplicitChildrenRecursive(node);

		setNodeVisibilityRecursiveBottomUp(node, noActive);
	}
}

void GraphController::setNodeActiveRecursive(DummyNode& node, const std::vector<Id>& activeTokenIds, bool* noActive) const
{
	node.active = false;

	if (node.isGraphNode())
	{
		node.active = find(activeTokenIds.begin(), activeTokenIds.end(), node.data->getId()) != activeTokenIds.end();

		if (node.active)
		{
			*noActive = false;
		}
	}

	for (DummyNode& subNode : node.subNodes)
	{
		setNodeActiveRecursive(subNode, activeTokenIds, noActive);
	}
}

void GraphController::removeImplicitChildrenRecursive(DummyNode& node)
{
	if (node.isGraphNode() && !node.data->isExplicit())
	{
		return;
	}

	for (size_t i = 0; i < node.subNodes.size(); i++)
	{
		bool removeNode = false;

		DummyNode& subNode = node.subNodes[i];
		if (subNode.isGraphNode() && subNode.data->isImplicit() && !subNode.connected && !subNode.active && !subNode.subNodes.size())
		{
			removeNode = true;
		}
		else
		{
			removeImplicitChildrenRecursive(subNode);

			if (subNode.isAccessNode() && subNode.subNodes.size() == 0)
			{
				removeNode = true;
			}
		}

		if (removeNode)
		{
			node.subNodes.erase(node.subNodes.begin() + i);
			i--;
		}
	}
}

bool GraphController::setNodeVisibilityRecursiveBottomUp(DummyNode& node, bool noActive) const
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
		if (setNodeVisibilityRecursiveBottomUp(subNode, noActive))
		{
			node.childVisible = true;
		}
	}

	if (noActive || node.active || node.connected || node.childVisible)
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
	bundleNodesAndEdgesMatching(
		[&](const DummyNode& node)
		{
			return isTypeNodeWithSingleAggregation(node, TokenComponentAggregation::DIRECTION_BACKWARD);
		},
		3,
		"Referenced Types"
	);

	// bundleNodesAndEdgesMatching(
	// 	[&](const DummyNode& node)
	// 	{
	// 		return isTypeNodeWithSingleAggregation(node, TokenComponentAggregation::DIRECTION_FORWARD);
	// 	},
	// 	3,
	// 	"Referencing Types"
	// );

	bundleNodesAndEdgesMatching(
		[&](const DummyNode& node)
		{
			return isTypeNodeWithSingleInheritance(node, true);
		},
		3,
		"Base Types"
	);

	bundleNodesAndEdgesMatching(
		[&](const DummyNode& node)
		{
			return isTypeNodeWithSingleInheritance(node, false);
		},
		3,
		"Derived Types"
	);

	bundleNodesAndEdgesMatching(
		[&](const DummyNode& node)
		{
			return isUndefinedNode(node, false);
		},
		2,
		"Undefined Symbols"
	);

	bundleNodesAndEdgesMatching(
		[&](const DummyNode& node)
		{
			return isUndefinedNode(node, true);
		},
		2,
		"Undefined Symbols"
	);

	bundleNodesAndEdgesMatching(
		[&](const DummyNode& node)
		{
			return isTypeUserNode(node);
		},
		8,
		"Referencing Symbols"
	);
}

void GraphController::bundleNodesAndEdgesMatching(std::function<bool(const DummyNode&)> matcher, size_t count, const std::string& name)
{
	std::vector<size_t> matchedNodeIndices;
	for (size_t i = 0; i < m_dummyNodes.size(); i++)
	{
		if (matcher(m_dummyNodes[i]))
		{
			matchedNodeIndices.push_back(i);
		}
	}

	if (!matchedNodeIndices.size() || matchedNodeIndices.size() < count || matchedNodeIndices.size() == m_dummyNodes.size())
	{
		return;
	}

	DummyNode bundleNode;
	bundleNode.name = name;
	bundleNode.visible = true;

	for (int i = matchedNodeIndices.size() - 1; i >= 0; i--)
	{
		DummyNode node = m_dummyNodes[matchedNodeIndices[i]];
		node.visible = false;

		bundleNode.bundledNodes.push_back(node);
		bundleNode.bundledNodeCount += node.getConnectedSubNodeCount();

		m_dummyNodes.erase(m_dummyNodes.begin() + matchedNodeIndices[i]);
	}

	bundleNode.tokenId = bundleNode.bundledNodes[0].data->getId();
	m_dummyNodes.push_back(bundleNode);

	if (m_dummyEdges.size() == 0)
	{
		return;
	}

	std::vector<DummyEdge> bundleEdges;
	std::vector<const DummyNode*> bundledNodes = bundleNode.getAllBundledNodes();
	for (const DummyNode* node : bundledNodes)
	{
		for (DummyEdge& edge : m_dummyEdges)
		{
			bool owner = (edge.ownerId == node->data->getId());
			bool target = (edge.targetId == node->data->getId());

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
}

void GraphController::bundleNodesMatching(std::list<DummyNode*>& nodes, std::function<bool(const DummyNode&)> matcher, const std::string& name)
{
	std::vector<std::list<DummyNode*>::iterator> matchedNodes;
	for (std::list<DummyNode*>::iterator it = nodes.begin(); it != nodes.end(); it++)
	{
		if (matcher(**it))
		{
			matchedNodes.push_back(it);
		}
	}

	if (!matchedNodes.size())
	{
		return;
	}

	DummyNode bundleNode;
	bundleNode.name = name;
	bundleNode.visible = true;

	for (int i = matchedNodes.size() - 1; i >= 0; i--)
	{
		DummyNode* node = *matchedNodes[i];
		node->visible = false;

		bundleNode.bundledNodes.push_back(*node);
		bundleNode.bundledNodeCount += node->getConnectedSubNodeCount();

		nodes.erase(matchedNodes[i]);
	}

	bundleNode.tokenId = bundleNode.bundledNodes[0].data->getId();
	m_dummyNodes.push_back(bundleNode);
}

bool GraphController::isTypeNodeWithSingleAggregation(
	const DummyNode& node, TokenComponentAggregation::Direction direction
) const {
	const Node::NodeTypeMask typeMask = Node::NODE_STRUCT | Node::NODE_CLASS | Node::NODE_TYPEDEF;

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
		matches = false;
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

bool GraphController::isUndefinedNode(const DummyNode& node, bool isUsed) const
{
	if (!node.visible || node.active || !node.isGraphNode() || node.hasActiveSubNode() || node.data->isDefined())
	{
		return false;
	}

	bool matches = true;
	Id tokenId = node.data->getId();

	node.data->forEachEdge(
		[isUsed, tokenId, &matches](Edge* edge)
		{
			if (edge->isType(Edge::EDGE_MEMBER))
			{
				return;
			}

			Id fromId = edge->getFrom()->getId();
			Id toId = edge->getTo()->getId();

			if (edge->isType(Edge::EDGE_AGGREGATION))
			{
				TokenComponentAggregation::Direction dir = edge->getComponent<TokenComponentAggregation>()->getDirection();

				switch (dir)
				{
				case TokenComponentAggregation::DIRECTION_BACKWARD:
					{
						Id id = fromId;
						fromId = toId;
						toId = id;
					}
					break;
				case TokenComponentAggregation::DIRECTION_NONE:
					matches = false;
					return;
				default:
					break;
				}
			}

			if ((!isUsed && toId == tokenId) ||
				(isUsed && fromId == tokenId))
			{
				matches = false;
			}
		}
	);

	return matches;
}

bool GraphController::isTypeUserNode(const DummyNode& node) const
{
	if (!node.visible || node.active || !node.isGraphNode() || node.hasActiveSubNode())
	{
		return false;
	}

	std::vector<const Node*> nodes;
	nodes.push_back(node.data);

	node.data->forEachChildNodeRecursive(
		[&nodes](Node* n)
		{
			nodes.push_back(n);
		}
	);

	bool matches = true;
	for (const Node* n : nodes)
	{
		Id tokenId = n->getId();

		n->forEachEdge(
			[tokenId, &matches](Edge* edge)
			{
				if (edge->isType(Edge::EDGE_MEMBER))
				{
					return;
				}

				if (edge->isType(Edge::EDGE_AGGREGATION))
				{
					TokenComponentAggregation::Direction dir =
						edge->getComponent<TokenComponentAggregation>()->getDirection();
					if ((dir == TokenComponentAggregation::DIRECTION_FORWARD && edge->getTo()->getId() == tokenId) ||
						(dir == TokenComponentAggregation::DIRECTION_BACKWARD && edge->getTo()->getId() != tokenId))
					{
						matches = false;
					}

					return;
				}

				if (!edge->isType(Edge::EDGE_TYPE_USAGE | Edge::EDGE_TYPE_OF | Edge::EDGE_TEMPLATE_ARGUMENT | Edge::EDGE_TYPEDEF_OF) ||
					edge->getTo()->getId() == tokenId)
				{
					matches = false;
				}
			}
		);
	}

	return matches;
}

#define BUNDLE_BY_TYPE(__nodes__, __type__, __name__) \
	bundleNodesMatching( \
		__nodes__, \
		[&](const DummyNode& node) \
		{ \
			return node.visible && node.isGraphNode() && node.data->isType(__type__); \
		}, \
		__name__ \
	); \

void GraphController::bundleNodesByType()
{
	std::vector<DummyNode> oldNodes = m_dummyNodes;
	m_dummyNodes.clear();

	std::list<DummyNode*> nodes;
	for (size_t i = 0; i < oldNodes.size(); i++)
	{
		nodes.push_back(&oldNodes[i]);
	}

	BUNDLE_BY_TYPE(nodes, Node::NODE_NAMESPACE, "Namespaces");
	BUNDLE_BY_TYPE(nodes, Node::NODE_CLASS, "Classes");
	BUNDLE_BY_TYPE(nodes, Node::NODE_STRUCT, "Structs");

	BUNDLE_BY_TYPE(nodes, Node::NODE_FUNCTION, "Functions");
	BUNDLE_BY_TYPE(nodes, Node::NODE_GLOBAL_VARIABLE, "Global Variables");

	BUNDLE_BY_TYPE(nodes, Node::NODE_TYPE, "Types");
	BUNDLE_BY_TYPE(nodes, Node::NODE_TYPEDEF, "Typedefs");
	BUNDLE_BY_TYPE(nodes, Node::NODE_ENUM, "Enums");

	BUNDLE_BY_TYPE(nodes, Node::NODE_FILE, "Files");
	BUNDLE_BY_TYPE(nodes, Node::NODE_MACRO, "Macros");

	// // should never be visible

	BUNDLE_BY_TYPE(nodes, Node::NODE_METHOD, "Methods");
	BUNDLE_BY_TYPE(nodes, Node::NODE_FIELD, "Fields");
	BUNDLE_BY_TYPE(nodes, Node::NODE_ENUM_CONSTANT, "Enum Constants");
	BUNDLE_BY_TYPE(nodes, Node::NODE_TEMPLATE_PARAMETER_TYPE, "Template Parameter Types");
	BUNDLE_BY_TYPE(nodes, Node::NODE_UNDEFINED, "Undefined Symbols");

	for (DummyNode& node : m_dummyNodes)
	{
		if (node.isBundleNode())
		{
			sort(node.bundledNodes.begin(), node.bundledNodes.end(),
				[](const DummyNode& a, const DummyNode& b) -> bool
				{
					return utility::toLowerCase(a.name) < utility::toLowerCase(b.name);
				}
			);
		}
	}
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
		size_t maxNameSize = 50;
		if (!node.active && node.name.size() > maxNameSize)
		{
			node.name = node.name.substr(0, maxNameSize - 3) + "...";
		}

		width = margins.charWidth * node.name.size();

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
	if (!message->isReplayed())
	{
		getView()->rebuildGraph(m_graph, m_dummyNodes, m_dummyEdges);
		m_graph.reset();
	}
}
