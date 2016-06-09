#include "component/controller/GraphController.h"

#include <set>

#include "utility/logging/logging.h"
#include "utility/utility.h"
#include "utility/utilityString.h"

#include "component/controller/helper/BucketGrid.h"
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
	layoutGraph(true);

	buildGraph(message);
}

void GraphController::handleMessage(MessageFlushUpdates* message)
{
	buildGraph(message);
}

void GraphController::handleMessage(MessageSearchFullText* message)
{
	clear();
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
		DummyNode* node = m_dummyNodes[i].get();
		if (node->isBundleNode() && node->tokenId == message->bundleId)
		{
			m_dummyNodes.insert(m_dummyNodes.begin() + i + 1, node->bundledNodes.begin(), node->bundledNodes.end());
			m_dummyNodes.erase(m_dummyNodes.begin() + i);
			break;
		}
	}

	for (size_t i = 0; i < m_dummyEdges.size(); i++)
	{
		DummyEdge* edge = m_dummyEdges[i].get();
		if (!edge->data && edge->targetId == message->bundleId)
		{
			m_dummyEdges.erase(m_dummyEdges.begin() + i);
			break;
		}
	}

	std::vector<Id> tokenIds = utility::concat(m_activeNodeIds, m_activeEdgeIds);
	setActiveAndVisibility(tokenIds);

	layoutNesting();
	layoutGraph(tokenIds.size() > 0);

	buildGraph(message);
}

void GraphController::handleMessage(MessageGraphNodeExpand* message)
{
	DummyNode* node = getDummyGraphNodeById(message->tokenId);
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
	DummyNode* node = getDummyGraphNodeById(message->tokenId);
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

	m_dummyGraphNodes.clear();

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

	std::vector<Id> expandedNodeIds = getExpandedNodeIds();

	m_dummyEdges.clear();
	m_dummyGraphNodes.clear();

	std::set<Id> addedNodes;
	std::vector<std::shared_ptr<DummyNode>> dummyNodes;

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

			dummyNodes.push_back(createDummyNodeTopDown(parent, parent->getId()));
		}
	);

	std::set<Id> addedEdges;
	graph->forEachEdge(
		[&addedEdges, this](Edge* edge)
		{
			if (!edge->isType(Edge::EDGE_MEMBER) && addedEdges.find(edge->getId()) == addedEdges.end())
			{
				m_dummyEdges.push_back(std::make_shared<DummyEdge>(edge->getFrom()->getId(), edge->getTo()->getId(), edge));
				addedEdges.insert(edge->getId());
			}
		}
	);

	for (std::shared_ptr<DummyNode> node : dummyNodes)
	{
		node->hasParent = false;
		node->name = node->data->getFullName();
	}

	m_dummyNodes = dummyNodes;

	bool noActive = setActive(tokenIds);

	autoExpandActiveNode(tokenIds);
	setExpandedNodeIds(expandedNodeIds);

	setVisibility(noActive);

	m_graph = graph;
}

std::shared_ptr<DummyNode> GraphController::createDummyNodeTopDown(Node* node, Id parentId)
{
	std::shared_ptr<DummyNode> result = std::make_shared<DummyNode>();
	result->data = node;
	result->tokenId = node->getId();
	result->name = node->getName();
	result->topLevelAncestorId = parentId;

	node->forEachChildNode(
		[node, &parentId, &result, this](Node* child)
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
					parent = result.get();
				}
			}

			if (accessType != TokenComponentAccess::ACCESS_NONE)
			{
				for (std::shared_ptr<DummyNode> dummy : result->subNodes)
				{
					if (dummy->accessType == accessType)
					{
						parent = dummy.get();
						break;
					}
				}

				if (!parent)
				{
					std::shared_ptr<DummyNode> accessNode = std::make_shared<DummyNode>();
					accessNode->accessType = accessType;
					result->subNodes.push_back(accessNode);
					parent = accessNode.get();
				}
			}

			parent->subNodes.push_back(createDummyNodeTopDown(child, parentId));
		}
	);

	m_dummyGraphNodes.emplace(result->data->getId(), result);

	return result;
}

std::vector<Id> GraphController::getExpandedNodeIds() const
{
	std::vector<Id> nodeIds;
	for (std::pair<Id, std::shared_ptr<DummyNode>> p : m_dummyGraphNodes)
	{
		DummyNode* oldNode = p.second.get();
		if (oldNode->expanded && oldNode->isGraphNode() && !oldNode->data->isType(Node::NODE_FUNCTION | Node::NODE_METHOD))
		{
			nodeIds.push_back(p.first);
		}
	}
	return nodeIds;
}

void GraphController::setExpandedNodeIds(const std::vector<Id>& nodeIds)
{
	for (Id id : nodeIds)
	{
		DummyNode* node = getDummyGraphNodeById(id);
		if (node && node->topLevelAncestorId)
		{
			DummyNode* parent = getDummyGraphNodeById(node->topLevelAncestorId);

			if (parent && parent->hasActiveSubNode())
			{
				node->expanded = true;
			}
		}
	}
}

void GraphController::autoExpandActiveNode(const std::vector<Id>& activeTokenIds)
{
	DummyNode* node = nullptr;
	if (activeTokenIds.size() == 1)
	{
		node = getDummyGraphNodeById(activeTokenIds[0]);
	}

	if (node)
	{
		node->expanded = true;
	}
}

bool GraphController::setActive(const std::vector<Id>& activeTokenIds)
{
	bool noActive = activeTokenIds.size() == 0;
	if (activeTokenIds.size() > 0)
	{
		noActive = true;
		for (std::shared_ptr<DummyNode> node : m_dummyNodes)
		{
			setNodeActiveRecursive(node.get(), activeTokenIds, &noActive);
		}
	}

	for (std::shared_ptr<DummyEdge> edge : m_dummyEdges)
	{
		if (!edge->data)
		{
			continue;
		}

		edge->active = false;
		if (find(activeTokenIds.begin(), activeTokenIds.end(), edge->data->getId()) != activeTokenIds.end())
		{
			edge->active = true;
			noActive = false;
		}

		DummyNode* from = getDummyGraphNodeById(edge->ownerId);
		DummyNode* to = getDummyGraphNodeById(edge->targetId);

		if (from && to && (noActive || from->active || to->active || edge->active))
		{
			edge->visible = true;
			from->connected = true;
			to->connected = true;
		}
	}

	return noActive;
}

void GraphController::setVisibility(bool noActive)
{
	for (std::shared_ptr<DummyNode> node : m_dummyNodes)
	{
		removeImplicitChildrenRecursive(node.get());

		setNodeVisibilityRecursiveBottomUp(node.get(), noActive);
	}
}

void GraphController::setActiveAndVisibility(const std::vector<Id>& activeTokenIds)
{
	setVisibility(setActive(activeTokenIds));
}

void GraphController::setNodeActiveRecursive(DummyNode* node, const std::vector<Id>& activeTokenIds, bool* noActive) const
{
	node->active = false;

	if (node->isGraphNode())
	{
		node->active = find(activeTokenIds.begin(), activeTokenIds.end(), node->data->getId()) != activeTokenIds.end();

		if (node->active)
		{
			*noActive = false;
		}
	}

	for (std::shared_ptr<DummyNode> subNode : node->subNodes)
	{
		setNodeActiveRecursive(subNode.get(), activeTokenIds, noActive);
	}
}

void GraphController::removeImplicitChildrenRecursive(DummyNode* node)
{
	if (node->isGraphNode() && !node->data->isExplicit())
	{
		return;
	}

	for (size_t i = 0; i < node->subNodes.size(); i++)
	{
		bool removeNode = false;

		DummyNode* subNode = node->subNodes[i].get();
		if (subNode->isGraphNode() && subNode->data->isImplicit() &&
			!subNode->connected && !subNode->active && !subNode->subNodes.size())
		{
			removeNode = true;
		}
		else
		{
			removeImplicitChildrenRecursive(subNode);

			if (subNode->isAccessNode() && subNode->subNodes.size() == 0)
			{
				removeNode = true;
			}
		}

		if (removeNode)
		{
			node->subNodes.erase(node->subNodes.begin() + i);
			i--;
		}
	}
}

bool GraphController::setNodeVisibilityRecursiveBottomUp(DummyNode* node, bool noActive) const
{
	node->visible = false;
	node->childVisible = false;

	if (node->isExpandToggleNode())
	{
		node->visible = true;
		return false;
	}
	else if (node->isBundleNode())
	{
		node->visible = true;
		return true;
	}

	for (std::shared_ptr<DummyNode> subNode : node->subNodes)
	{
		if (setNodeVisibilityRecursiveBottomUp(subNode.get(), noActive))
		{
			node->childVisible = true;
		}
	}

	if (noActive || node->active || node->connected || node->childVisible)
	{
		setNodeVisibilityRecursiveTopDown(node, false);
	}

	return node->visible;
}

void GraphController::setNodeVisibilityRecursiveTopDown(DummyNode* node, bool parentExpanded) const
{
	node->visible = true;

	if ((node->isGraphNode() && node->isExpanded()) ||
		(node->isAccessNode() && parentExpanded) ||
		(node->isGraphNode() && parentExpanded && node->data->isType(Node::NODE_ENUM_CONSTANT)))
	{
		for (std::shared_ptr<DummyNode> subNode : node->subNodes)
		{
			node->childVisible = true;
			setNodeVisibilityRecursiveTopDown(subNode.get(), node->isExpanded());
		}
	}
}

void GraphController::bundleNodes()
{
	// evaluate top level nodes
	for (std::shared_ptr<DummyNode> node : m_dummyNodes)
	{
		if (!node->isGraphNode() || !node->visible)
		{
			continue;
		}

		DummyNode::BundleInfo* bundleInfo = &node->bundleInfo;
		bundleInfo->isActive = node->hasActiveSubNode();

		node->data->forEachNodeRecursive(
			[&bundleInfo](const Node* n)
			{
				if (n->isDefined())
				{
					bundleInfo->isDefined = true;
				}

				if (bundleInfo->layoutVertical)
				{
					return;
				}

				n->forEachEdgeOfType(
					~Edge::EDGE_MEMBER,
					[&bundleInfo, &n](Edge* e)
					{
						if (bundleInfo->layoutVertical)
						{
							return;
						}

						if (e->isType(Edge::EDGE_INHERITANCE | Edge::EDGE_OVERRIDE))
						{
							bundleInfo->layoutVertical = true;
							bundleInfo->isReferenced = false;
							bundleInfo->isReferencing = false;
						}

						if (e->isType(Edge::EDGE_AGGREGATION))
						{
							TokenComponentAggregation::Direction dir =
								e->getComponent<TokenComponentAggregation>()->getDirection();

							if (dir == TokenComponentAggregation::DIRECTION_NONE)
							{
								bundleInfo->isReferenced = true;
								bundleInfo->isReferencing = true;
							}
							else if ((dir == TokenComponentAggregation::DIRECTION_FORWARD && e->getFrom() == n) ||
								(dir == TokenComponentAggregation::DIRECTION_BACKWARD && e->getTo() == n))
							{
								bundleInfo->isReferencing = true;
							}
							else if ((dir == TokenComponentAggregation::DIRECTION_FORWARD && e->getTo() == n) ||
								(dir == TokenComponentAggregation::DIRECTION_BACKWARD && e->getFrom() == n))
							{
								bundleInfo->isReferenced = true;
							}
						}
						else
						{
							if (e->getTo() == n)
							{
								bundleInfo->isReferenced = true;
							}
							else if (e->getFrom() == n)
							{
								bundleInfo->isReferencing = true;
							}
						}
					}
				);
			}
		);

		if (bundleInfo->isReferenced && bundleInfo->isReferencing)
		{
			bundleInfo->isReferenced = false;
			bundleInfo->isReferencing = false;
		}

		if (bundleInfo->isActive)
		{
			bundleInfo->layoutVertical = false;
		}
	}

	// Left for debugging
	// for (std::shared_ptr<DummyNode> node : m_dummyNodes)
	// {
	// 	std::cout << node->bundleInfo.isActive << " ";
	// 	std::cout << node->bundleInfo.isDefined << " ";
	// 	std::cout << node->bundleInfo.layoutVertical << " ";
	// 	std::cout << node->bundleInfo.isReferenced << " ";
	// 	std::cout << node->bundleInfo.isReferencing << " ";
	// 	std::cout << node->name << std::endl;
	// }

	// bundle
	bundleNodesAndEdgesMatching(
		[](const DummyNode::BundleInfo& info)
		{
			return !info.isDefined && info.isReferencing && !info.layoutVertical;
		},
		2,
		"Undefined Symbols"
	);

	bundleNodesAndEdgesMatching(
		[](const DummyNode::BundleInfo& info)
		{
			return !info.isDefined && info.isReferenced && !info.layoutVertical;
		},
		2,
		"Undefined Symbols"
	);

	bundleNodesAndEdgesMatching(
		[](const DummyNode::BundleInfo& info)
		{
			return info.isDefined && info.isReferencing && !info.layoutVertical;
		},
		10,
		"Referencing Symbols"
	);

	bundleNodesAndEdgesMatching(
		[](const DummyNode::BundleInfo& info)
		{
			return info.isDefined && info.isReferenced && !info.layoutVertical;
		},
		10,
		"Referenced Symbols"
	);

	bundleNodesAndEdgesMatching(
		[](const DummyNode::BundleInfo& info)
		{
			return info.isReferencing && info.layoutVertical;
		},
		5,
		"Derived Symbols"
	);

	bundleNodesAndEdgesMatching(
		[](const DummyNode::BundleInfo& info)
		{
			return info.isReferenced && info.layoutVertical;
		},
		5,
		"Base Symbols"
	);
}

void GraphController::bundleNodesAndEdgesMatching(
	std::function<bool(const DummyNode::BundleInfo&)> matcher, size_t count, const std::string& name
){
	std::vector<size_t> matchedNodeIndices;
	for (size_t i = 0; i < m_dummyNodes.size(); i++)
	{
		const DummyNode* node = m_dummyNodes[i].get();
		if (node->bundleInfo.isActive || !node->visible)
		{
			continue;
		}

		if (matcher(node->bundleInfo))
		{
			matchedNodeIndices.push_back(i);
		}
	}

	if (!matchedNodeIndices.size() || matchedNodeIndices.size() < count || matchedNodeIndices.size() == m_dummyNodes.size())
	{
		return;
	}

	std::shared_ptr<DummyNode> bundleNode = std::make_shared<DummyNode>();
	bundleNode->name = name;
	bundleNode->visible = true;

	for (int i = matchedNodeIndices.size() - 1; i >= 0; i--)
	{
		std::shared_ptr<DummyNode> node = m_dummyNodes[matchedNodeIndices[i]];
		node->visible = false;

		bundleNode->bundledNodes.push_back(node);
		bundleNode->bundledNodeCount += node->getConnectedSubNodeCount();

		m_dummyNodes.erase(m_dummyNodes.begin() + matchedNodeIndices[i]);
	}

	DummyNode* firstNode = bundleNode->bundledNodes[0].get();
	bundleNode->tokenId = firstNode->data->getId();
	bundleNode->bundleInfo.layoutVertical = firstNode->bundleInfo.layoutVertical;
	bundleNode->bundleInfo.isReferenced = firstNode->bundleInfo.isReferenced;
	bundleNode->bundleInfo.isReferencing = firstNode->bundleInfo.isReferencing;
	m_dummyNodes.push_back(bundleNode);

	if (m_dummyEdges.size() == 0)
	{
		return;
	}

	std::vector<std::shared_ptr<DummyEdge>> bundleEdges;
	std::vector<const DummyNode*> bundledNodes = bundleNode->getAllBundledNodes();
	for (const DummyNode* node : bundledNodes)
	{
		for (std::shared_ptr<DummyEdge> edge : m_dummyEdges)
		{
			bool owner = (edge->ownerId == node->data->getId());
			bool target = (edge->targetId == node->data->getId());

			if (!owner && !target)
			{
				continue;
			}

			DummyEdge* bundleEdgePtr = nullptr;
			for (std::shared_ptr<DummyEdge> bundleEdge : bundleEdges)
			{
				if ((owner && bundleEdge->ownerId == edge->targetId) ||
					(target && bundleEdge->ownerId == edge->ownerId))
				{
					bundleEdgePtr = bundleEdge.get();
					break;
				}
			}

			if (!bundleEdgePtr)
			{
				std::shared_ptr<DummyEdge> bundleEdge = std::make_shared<DummyEdge>();
				bundleEdge->visible = true;
				bundleEdge->ownerId = (owner ? edge->targetId : edge->ownerId);
				bundleEdge->targetId = bundleNode->tokenId;
				bundleEdges.push_back(bundleEdge);
				bundleEdgePtr = bundleEdges.back().get();
			}

			bundleEdgePtr->weight += edge->getWeight();
			bundleEdgePtr->updateDirection(edge->getDirection(), owner);
			edge->visible = false;
		}
	}

	m_dummyEdges.insert(m_dummyEdges.end(), bundleEdges.begin(), bundleEdges.end());
}

void GraphController::bundleNodesMatching(
	std::list<std::shared_ptr<DummyNode>>& nodes, std::function<bool(const DummyNode*)> matcher, const std::string& name
){
	std::vector<std::list<std::shared_ptr<DummyNode>>::iterator> matchedNodes;
	for (std::list<std::shared_ptr<DummyNode>>::iterator it = nodes.begin(); it != nodes.end(); it++)
	{
		if (matcher(it->get()))
		{
			matchedNodes.push_back(it);
		}
	}

	if (!matchedNodes.size())
	{
		return;
	}

	std::shared_ptr<DummyNode> bundleNode = std::make_shared<DummyNode>();
	bundleNode->name = name;
	bundleNode->visible = true;

	for (int i = matchedNodes.size() - 1; i >= 0; i--)
	{
		std::shared_ptr<DummyNode> node = *matchedNodes[i];
		node->visible = false;

		bundleNode->bundledNodes.push_back(node);
		bundleNode->bundledNodeCount += node->getConnectedSubNodeCount();

		nodes.erase(matchedNodes[i]);
	}

	bundleNode->tokenId = bundleNode->bundledNodes[0]->data->getId();
	m_dummyNodes.push_back(bundleNode);
}

#define BUNDLE_BY_TYPE(__nodes__, __type__, __name__) \
	bundleNodesMatching( \
		__nodes__, \
		[&](const DummyNode* node) \
		{ \
			return node->visible && node->isGraphNode() && node->data->isType(__type__); \
		}, \
		__name__ \
	); \

void GraphController::bundleNodesByType()
{
	std::vector<std::shared_ptr<DummyNode>> oldNodes = m_dummyNodes;
	m_dummyNodes.clear();

	std::list<std::shared_ptr<DummyNode>> nodes;
	for (size_t i = 0; i < oldNodes.size(); i++)
	{
		nodes.push_back(oldNodes[i]);
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

	for (std::shared_ptr<DummyNode> node : m_dummyNodes)
	{
		if (node->isBundleNode())
		{
			sort(node->bundledNodes.begin(), node->bundledNodes.end(),
				[](const std::shared_ptr<DummyNode> a, const std::shared_ptr<DummyNode> b) -> bool
				{
					return utility::toLowerCase(a->name) < utility::toLowerCase(b->name);
				}
			);
		}
	}
}

void GraphController::layoutNesting()
{
	for (std::shared_ptr<DummyNode> node : m_dummyNodes)
	{
		layoutNestingRecursive(node.get());
	}

	for (std::shared_ptr<DummyNode> node : m_dummyNodes)
	{
		layoutToGrid(node.get());
	}
}

void GraphController::layoutNestingRecursive(DummyNode* node) const
{
	if (!node->visible)
	{
		return;
	}

	GraphViewStyle::NodeMargins margins;

	if (node->isGraphNode())
	{
		margins = GraphViewStyle::getMarginsForNodeType(node->data->getType(), node->childVisible);
	}
	else if (node->isAccessNode())
	{
		margins = GraphViewStyle::getMarginsOfAccessNode(node->accessType);
	}
	else if (node->isExpandToggleNode())
	{
		margins = GraphViewStyle::getMarginsOfExpandToggleNode();
	}
	else if (node->isBundleNode())
	{
		margins = GraphViewStyle::getMarginsOfBundleNode();
	}

	int y = 0;
	int x = 0;
	int width = margins.minWidth;
	int height = 0;

	if (node->isGraphNode())
	{
		size_t maxNameSize = 50;
		if (!node->active && node->name.size() > maxNameSize)
		{
			node->name = node->name.substr(0, maxNameSize - 3) + "...";
		}

		width = margins.charWidth * node->name.size();

		if (node->data->isType(Node::NODE_TYPE | Node::NODE_CLASS | Node::NODE_STRUCT | Node::NODE_ENUM) &&
			node->subNodes.size())
		{
			addExpandToggleNode(node);
		}
	}
	else if (node->isBundleNode())
	{
		width = margins.charWidth * node->name.size();
	}

	width += margins.iconWidth;

	// Horizontal layouting is currently not used, but left in place for experimentation.
	bool layoutHorizontal = false;

	for (std::shared_ptr<DummyNode> subNode : node->subNodes)
	{
		if (!subNode->visible)
		{
			continue;
		}

		layoutNestingRecursive(subNode.get());

		if (subNode->isExpandToggleNode())
		{
			width += margins.spacingX + subNode->size.x;
		}
	}

	for (std::shared_ptr<DummyNode> subNode : node->subNodes)
	{
		if (!subNode->visible || subNode->isExpandToggleNode())
		{
			continue;
		}

		subNode->position.x = margins.left + x;
		subNode->position.y = margins.top + margins.charHeight + y;

		if (layoutHorizontal)
		{
			x += subNode->size.x + margins.spacingX;
			if (subNode->size.y > height)
			{
				height = subNode->size.y;
			}
		}
		else
		{
			y += subNode->size.y + margins.spacingY;
			if (subNode->size.x > width)
			{
				width = subNode->size.x;
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

	node->size.x = margins.left + width + margins.right;
	node->size.y = margins.top + margins.charHeight + y + height + margins.bottom;

	for (std::shared_ptr<DummyNode> subNode : node->subNodes)
	{
		if (!subNode->visible)
		{
			continue;
		}

		if (subNode->isAccessNode())
		{
			subNode->size.x = width;
		}
		else if (subNode->isExpandToggleNode())
		{
			subNode->position.x = margins.left + width - subNode->size.x;
			subNode->position.y = 6;
		}
	}
}

void GraphController::addExpandToggleNode(DummyNode* node) const
{
	std::shared_ptr<DummyNode> expandNode = std::make_shared<DummyNode>();
	expandNode->visible = true;
	expandNode->expanded = node->expanded;

	for (size_t i = 0; i < node->subNodes.size(); i++)
	{
		DummyNode* subNode = node->subNodes[i].get();

		if (subNode->isExpandToggleNode())
		{
			node->subNodes.erase(node->subNodes.begin() + i);
			i--;
			continue;
		}
		else if (subNode->isGraphNode() && subNode->data->isType(Node::NODE_ENUM_CONSTANT) && !subNode->visible)
		{
			expandNode->invisibleSubNodeCount++;
			continue;
		}

		for (std::shared_ptr<DummyNode> subSubNode : subNode->subNodes)
		{
			if (!subSubNode->visible)
			{
				expandNode->invisibleSubNodeCount++;
			}
		}
	}

	if (expandNode->isExpanded() || expandNode->invisibleSubNodeCount)
	{
		node->subNodes.push_back(expandNode);
	}
}

void GraphController::layoutToGrid(DummyNode* node) const
{
	if (!node->visible)
	{
		return;
	}

	size_t width = GraphViewStyle::toGridSize(node->size.x);
	size_t height = GraphViewStyle::toGridSize(node->size.y);

	size_t incX = width - node->size.x;
	size_t incY = height - node->size.y;

	node->size.x = width;
	node->size.y = height;

	if (!node->isGraphNode())
	{
		return;
	}

	DummyNode* lastAccessNode = nullptr;
	for (std::shared_ptr<DummyNode> subNode : node->subNodes)
	{
		if (!subNode->visible)
		{
			continue;
		}

		if (subNode->isAccessNode())
		{
			subNode->size.x = subNode->size.x + incX;
			lastAccessNode = subNode.get();
		}
		else if (subNode->isExpandToggleNode())
		{
			subNode->position.x = subNode->position.x + incX;
		}
	}

	if (lastAccessNode)
	{
		lastAccessNode->size.y = lastAccessNode->size.y + incY;
	}
}

void GraphController::layoutGraph(bool sort)
{
	BucketGrid grid(getView()->getViewSize());
	grid.createBuckets(m_dummyNodes, m_dummyEdges);

	if (sort)
	{
		grid.sortBuckets();
	}

	grid.layoutBuckets();
}

DummyNode* GraphController::getDummyGraphNodeById(Id tokenId) const
{
	std::map<Id, std::shared_ptr<DummyNode>>::const_iterator it = m_dummyGraphNodes.find(tokenId);
	if (it != m_dummyGraphNodes.end())
	{
		return it->second.get();
	}

	return nullptr;
}

void GraphController::buildGraph(MessageBase* message)
{
	if (!message->isReplayed())
	{
		getView()->rebuildGraph(m_graph, m_dummyNodes, m_dummyEdges);
	}
}
