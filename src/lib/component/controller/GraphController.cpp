#include "component/controller/GraphController.h"

#include <set>

#include "utility/logging/logging.h"
#include "utility/tracing.h"
#include "utility/utility.h"
#include "utility/utilityString.h"

#include "component/controller/helper/BucketLayouter.h"
#include "component/controller/helper/ListLayouter.h"
#include "component/view/GraphView.h"
#include "component/view/GraphViewStyle.h"
#include "data/access/StorageAccess.h"
#include "data/parser/AccessKind.h"
#include "data/graph/Graph.h"
#include "Application.h"

GraphController::GraphController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
{
}

GraphController::~GraphController()
{
}

void GraphController::handleMessage(MessageActivateAll* message)
{
	TRACE("graph all");

	m_activeNodeIds.clear();
	m_activeEdgeIds.clear();

	createDummyGraphForTokenIdsAndSetActiveAndVisibility(std::vector<Id>(), m_storageAccess->getGraphForAll());

	bundleNodesByType();

	layoutNesting();
	assignBundleIds();
	layoutGraph();

	buildGraph(message, false);
}

void GraphController::handleMessage(MessageActivateTokens* message)
{
	TRACE("graph activate");

	if (message->isEdge || message->keepContent())
	{
		m_activeEdgeIds = message->tokenIds;
		setActiveAndVisibility(utility::concat(m_activeNodeIds, m_activeEdgeIds));
		buildGraph(message, false, false);
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

	bool isNamespace = false;
	std::shared_ptr<Graph> graph = m_storageAccess->getGraphForActiveTokenIds(tokenIds, &isNamespace);

	createDummyGraphForTokenIdsAndSetActiveAndVisibility(tokenIds, graph);

	if (isNamespace)
	{
		addCharacterIndex();
		layoutNesting();
		layoutList();
	}
	else
	{
		if (m_activeNodeIds.size() == 1)
		{
			bundleNodes();
		}

		layoutNesting();
		layoutGraph(true);
		assignBundleIds();
	}

	buildGraph(message, !isNamespace, true, isNamespace);
}

void GraphController::handleMessage(MessageFlushUpdates* message)
{
	buildGraph(message, true, !message->keepContent());
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
			if (message->removeOtherNodes)
			{
				std::vector<std::shared_ptr<DummyNode>> nodes(node->bundledNodes.begin(), node->bundledNodes.end());
				m_dummyNodes = nodes;
			}
			else
			{
				m_dummyNodes.insert(m_dummyNodes.begin() + i + 1, node->bundledNodes.begin(), node->bundledNodes.end());
				m_dummyNodes.erase(m_dummyNodes.begin() + i);
			}
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

	if (message->layoutToList)
	{
		addCharacterIndex();
		layoutNesting();
		layoutList();
	}
	else
	{
		layoutNesting();
		layoutGraph();
	}

	buildGraph(message, false, true, message->layoutToList);
}

void GraphController::handleMessage(MessageGraphNodeExpand* message)
{
	if (message->ignoreIfNotReplayed && !message->isReplayed())
	{
		return;
	}

	DummyNode* node = getDummyGraphNodeById(message->tokenId);
	if (node)
	{
		if (!node->active && message->expand)
		{
			for (size_t i = 0, l = m_dummyEdges.size(); i < l; i++)
			{
				std::shared_ptr<DummyEdge> edge = m_dummyEdges[i];

				if (edge && edge->data && edge->data->isType(Edge::EDGE_AGGREGATION) &&
					(edge->targetId == node->tokenId || edge->ownerId == node->tokenId))
				{
					std::vector<Id> aggregationIds =
						utility::toVector<Id>(edge->data->getComponent<TokenComponentAggregation>()->getAggregationIds());

					if (m_graph->getEdgeById(aggregationIds[0]) != nullptr)
					{
						break;
					}

					std::shared_ptr<Graph> graph = m_storageAccess->getGraphForActiveTokenIds(aggregationIds);

					graph->forEachEdge(
						[this](Edge* e)
						{
							if (!e->isType(Edge::EDGE_MEMBER))
							{
								m_dummyEdges.push_back(std::make_shared<DummyEdge>(
									e->getFrom()->getId(), e->getTo()->getId(), m_graph->addEdgeAsPlainCopy(e)));
							}
						}
					);
				}
			}
		}

		node->expanded = message->expand;

		setActiveAndVisibility(utility::concat(m_activeNodeIds, m_activeEdgeIds));

		layoutNesting();
		layoutGraph();

		buildGraph(message, false);
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
			buildGraph(message, false);
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

void GraphController::handleMessage(MessageShowReference* message)
{
	if (!message->tokenId)
	{
		return;
	}

	m_activeEdgeIds = std::vector<Id>(1, message->tokenId);
	setActiveAndVisibility(utility::concat(m_activeNodeIds, m_activeEdgeIds));
	buildGraph(message, false, false);
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
	TRACE();

	GraphView* view = getView();
	if (!view)
	{
		LOG_ERROR("GraphController has no associated GraphView");
		return;
	}

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

		if (node->data->isType(Node::NODE_NAMESPACE | Node::NODE_PACKAGE))
		{
			node->name = node->data->getFullName();
		}
		else
		{
			node->name = node->data->getName();

			NameHierarchy qualifier = node->data->getNameHierarchy();
			qualifier.pop();

			if (qualifier.size())
			{
				std::shared_ptr<DummyNode> qualifierNode = std::make_shared<DummyNode>();
				qualifierNode->visible = true;
				qualifierNode->qualifierName = qualifier;

				node->subNodes.push_back(qualifierNode);
				node->hasQualifier = true;
			}
		}
	}

	m_dummyNodes = dummyNodes;

	m_graph = graph;
}

void GraphController::createDummyGraphForTokenIdsAndSetActiveAndVisibility(
	const std::vector<Id>& tokenIds, const std::shared_ptr<Graph> graph
){
	std::vector<Id> expandedNodeIds = getExpandedNodeIds();

	createDummyGraphForTokenIds(tokenIds, graph);

	bool noActive = setActive(tokenIds);

	autoExpandActiveNode(tokenIds);
	setExpandedNodeIds(expandedNodeIds);

	setVisibility(noActive);
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
			AccessKind accessKind = ACCESS_NONE;

			TokenComponentAccess* access = child->getComponent<TokenComponentAccess>();
			if (access)
			{
				accessKind = access->getAccess();
			}

			for (std::shared_ptr<DummyNode> dummy : result->subNodes)
			{
				if (dummy->accessKind == accessKind)
				{
					parent = dummy.get();
					break;
				}
			}

			if (!parent)
			{
				std::shared_ptr<DummyNode> accessNode = std::make_shared<DummyNode>();
				accessNode->accessKind = accessKind;
				accessNode->isAccess = true;
				result->subNodes.push_back(accessNode);
				parent = accessNode.get();
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
		if (oldNode->expanded && !oldNode->autoExpanded && oldNode->isGraphNode() &&
			!oldNode->data->isType(Node::NODE_FUNCTION | Node::NODE_METHOD))
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
		if (node)
		{
			node->expanded = true;
			MessageGraphNodeExpand(id, true, true);
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
		node->autoExpanded = true;
	}
}

bool GraphController::setActive(const std::vector<Id>& activeTokenIds)
{
	TRACE();

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
	TRACE();

	for (std::shared_ptr<DummyNode> node : m_dummyNodes)
	{
		removeImplicitChildrenRecursive(node.get());

		setNodeVisibilityRecursiveBottomUp(node.get(), noActive);
	}
}

void GraphController::setActiveAndVisibility(const std::vector<Id>& activeTokenIds)
{
	TRACE();

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
	else if (node->isQualifierNode())
	{
		node->visible = true;
		return false;
	}

	for (std::shared_ptr<DummyNode> subNode : node->subNodes)
	{
		if (setNodeVisibilityRecursiveBottomUp(subNode.get(), noActive))
		{
			node->childVisible = true;
		}
	}

	if (node->isAccessNode() && node->accessKind == ACCESS_NONE && node->childVisible)
	{
		node->visible = true;
	}
	else if (noActive || node->active || node->connected || node->childVisible)
	{
		setNodeVisibilityRecursiveTopDown(node, false);
	}

	return node->visible;
}

void GraphController::setNodeVisibilityRecursiveTopDown(DummyNode* node, bool parentExpanded) const
{
	node->visible = true;

	if (node->isGraphNode() && node->data->getType() == Node::NODE_ENUM && !node->isExpanded())
	{
		return;
	}

	if ((node->isGraphNode() && node->isExpanded()) ||
		(node->isAccessNode() && (node->accessKind == ACCESS_NONE || parentExpanded)))
	{
		for (std::shared_ptr<DummyNode> subNode : node->subNodes)
		{
			if (!subNode->isQualifierNode())
			{
				node->childVisible = true;
				setNodeVisibilityRecursiveTopDown(subNode.get(), node->isExpanded());
			}
		}
	}
}

void GraphController::bundleNodes()
{
	TRACE();

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
	bool fileOrMacroActive = false;
	for (std::shared_ptr<DummyNode> node : m_dummyNodes)
	{
		if (node->bundleInfo.isActive && (
			node->data->isType(Node::NODE_FILE | Node::NODE_MACRO) ||
			node->data->findEdgeOfType(Edge::EDGE_INCLUDE | Edge::EDGE_MACRO_USAGE) != nullptr))
		{
			fileOrMacroActive = true;
			break;
		}
	}

	if (!fileOrMacroActive)
	{
		bundleNodesAndEdgesMatching(
			[](const DummyNode::BundleInfo& info, const Node* data)
			{
				return data->isType(Node::NODE_FILE);
			},
			1,
			false,
			"Importing Files"
		);
	}

	bundleNodesAndEdgesMatching(
		[](const DummyNode::BundleInfo& info, const Node* data)
		{
			return !info.isDefined && info.isReferencing && !info.layoutVertical;
		},
		2,
		true,
		"Non-indexed Symbols"
	);

	bundleNodesAndEdgesMatching(
		[](const DummyNode::BundleInfo& info, const Node* data)
		{
			return !info.isDefined && info.isReferenced && !info.layoutVertical;
		},
		2,
		true,
		"Non-indexed Symbols"
	);

	bundleNodesAndEdgesMatching(
		[](const DummyNode::BundleInfo& info, const Node* data)
		{
			return info.isDefined && info.isReferenced && data->isType(Node::NODE_BUILTIN_TYPE);
		},
		3,
		false,
		"Built-in Types"
	);

	if (!fileOrMacroActive)
	{
		bundleNodesAndEdgesMatching(
			[](const DummyNode::BundleInfo& info, const Node* data)
			{
				return info.isDefined && info.isReferencing && !info.layoutVertical;
			},
			10,
			false,
			"Referencing Symbols"
		);

		bundleNodesAndEdgesMatching(
			[](const DummyNode::BundleInfo& info, const Node* data)
			{
				return info.isDefined && info.isReferenced && !info.layoutVertical;
			},
			10,
			false,
			"Referenced Symbols"
		);
	}

	bundleNodesAndEdgesMatching(
		[](const DummyNode::BundleInfo& info, const Node* data)
		{
			return info.isReferencing && info.layoutVertical;
		},
		5,
		false,
		"Derived Symbols"
	);

	bundleNodesAndEdgesMatching(
		[](const DummyNode::BundleInfo& info, const Node* data)
		{
			return info.isReferenced && info.layoutVertical;
		},
		5,
		false,
		"Base Symbols"
	);
}

void GraphController::bundleNodesAndEdgesMatching(
	std::function<bool(const DummyNode::BundleInfo&,
	const Node* data)> matcher,
	size_t count,
	bool countConnectedNodes,
	const std::string& name
){
	std::vector<size_t> matchedNodeIndices;
	size_t connectedNodeCount = 0;
	for (size_t i = 0; i < m_dummyNodes.size(); i++)
	{
		const DummyNode* node = m_dummyNodes[i].get();
		if (node->bundleInfo.isActive || !node->visible || !node->isGraphNode())
		{
			continue;
		}

		if (matcher(node->bundleInfo, node->data))
		{
			matchedNodeIndices.push_back(i);

			if (countConnectedNodes)
			{
				connectedNodeCount += node->getConnectedSubNodes().size();
			}
		}
	}

	size_t matchedNodeCount = countConnectedNodes ? connectedNodeCount : matchedNodeIndices.size();
	if (!matchedNodeIndices.size() || matchedNodeCount < count || matchedNodeIndices.size() == m_dummyNodes.size())
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

		bundleNode->bundledNodes.insert(node);
		bundleNode->bundledNodeCount += node->getBundledNodeCount();

		m_dummyNodes.erase(m_dummyNodes.begin() + matchedNodeIndices[i]);
	}

	if (countConnectedNodes)
	{
		bundleNode->bundledNodeCount = connectedNodeCount;
	}

	DummyNode* firstNode = bundleNode->bundledNodes.begin()->get();

	// Use token Id of first node and make first bit 1
	bundleNode->tokenId = ~(~size_t(0) >> 1) + firstNode->data->getId();
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

std::shared_ptr<DummyNode> GraphController::bundleNodesMatching(
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
		return nullptr;
	}

	std::shared_ptr<DummyNode> bundleNode = std::make_shared<DummyNode>();
	bundleNode->name = name;
	bundleNode->visible = true;

	for (int i = matchedNodes.size() - 1; i >= 0; i--)
	{
		std::shared_ptr<DummyNode> node = *matchedNodes[i];
		node->visible = false;

		bundleNode->bundledNodes.insert(node);
		nodes.erase(matchedNodes[i]);
	}

	// Use token Id of first node and make first bit 1
	bundleNode->tokenId = ~(~size_t(0) >> 1) + (*bundleNode->bundledNodes.begin())->data->getId();
	return bundleNode;
}

void GraphController::bundleByType(
	std::list<std::shared_ptr<DummyNode>>& nodes, Node::NodeType type, const std::string& name)
{
	std::shared_ptr<DummyNode> bundleNode = bundleNodesMatching(
		nodes,
		[&](const DummyNode* node)
		{
			return node->visible && node->isGraphNode() && node->data->isType(type);
		},
		name
	);

	if (bundleNode)
	{
		bundleNode->bundledNodeType = type;
		m_dummyNodes.push_back(bundleNode);
	}
}

void GraphController::bundleNodesByType()
{
	TRACE();

	std::vector<std::shared_ptr<DummyNode>> oldNodes = m_dummyNodes;
	m_dummyNodes.clear();

	std::list<std::shared_ptr<DummyNode>> nodes;
	for (size_t i = 0; i < oldNodes.size(); i++)
	{
		nodes.push_back(oldNodes[i]);
	}

	bundleByType(nodes, Node::NODE_FILE, "Files");
	bundleByType(nodes, Node::NODE_MACRO, "Macros");

	bundleByType(nodes, Node::NODE_NAMESPACE, "Namespaces");
	bundleByType(nodes, Node::NODE_PACKAGE, "Packages");

	// bundleByType(nodes, Node::NODE_BUILTIN_TYPE, "Built-in Types");
	bundleByType(nodes, Node::NODE_CLASS, "Classes");
	bundleByType(nodes, Node::NODE_INTERFACE, "Interfaces");
	bundleByType(nodes, Node::NODE_STRUCT, "Structs");

	bundleByType(nodes, Node::NODE_FUNCTION, "Functions");
	bundleByType(nodes, Node::NODE_GLOBAL_VARIABLE, "Global Variables");

	bundleByType(nodes, Node::NODE_TYPE, "Types");
	bundleByType(nodes, Node::NODE_TYPEDEF, "Typedefs");
	bundleByType(nodes, Node::NODE_ENUM, "Enums");

	// // should never be visible

	bundleByType(nodes, Node::NODE_METHOD, "Methods");
	bundleByType(nodes, Node::NODE_FIELD, "Fields");
	bundleByType(nodes, Node::NODE_ENUM_CONSTANT, "Enum Constants");
	bundleByType(nodes, Node::NODE_TEMPLATE_PARAMETER_TYPE, "Template Parameter Types");
	bundleByType(nodes, Node::NODE_TYPE_PARAMETER, "Type Parameters");
	bundleByType(nodes, Node::NODE_NON_INDEXED, "Non-indexed Symbols");

	if (nodes.size())
	{
		LOG_ERROR("Nodes left after bundling for overview");
	}

	for (std::shared_ptr<DummyNode> bundleNode : m_dummyNodes)
	{
		if (!bundleNode->isBundleNode())
		{
			LOG_ERROR("Non-Bundle node in overview");
		}

		if (bundleNode->name == "Namespaces")
		{
			std::list<std::shared_ptr<DummyNode>> nodes;
			for (std::shared_ptr<DummyNode> node : bundleNode->bundledNodes)
			{
				nodes.push_back(node);
			}
			bundleNode->bundledNodes.clear();

			std::shared_ptr<DummyNode> anonymousBundle = bundleNodesMatching(
				nodes,
				[&](const DummyNode* node)
				{
					return utility::isPrefix("anonymous", node->name);
				},
				"Anonymous Namespaces"
			);

			for (std::shared_ptr<DummyNode> node : nodes)
			{
				bundleNode->bundledNodes.insert(node);
			}

			if (anonymousBundle)
			{
				anonymousBundle->bundledNodeType = Node::NODE_NAMESPACE;
				bundleNode->bundledNodeCount = bundleNode->getBundledNodeCount() + anonymousBundle->getBundledNodeCount();
				bundleNode->bundledNodes.insert(anonymousBundle);
			}
		}
	}
}

void GraphController::addCharacterIndex()
{
	// Remove index characters from last time
	DummyNode::BundledNodesSet newNodes;
	for (const std::shared_ptr<DummyNode> node : m_dummyNodes)
	{
		if (!node->isTextNode())
		{
			newNodes.insert(node);
		}
	}
	m_dummyNodes.clear();
	m_dummyNodes.insert(m_dummyNodes.end(), newNodes.begin(), newNodes.end());

	// Add index characters
	char character = 0;
	for (size_t i = 0; i < m_dummyNodes.size(); i++)
	{
		if (!m_dummyNodes[i]->name.size())
		{
			continue;
		}

		if (toupper(m_dummyNodes[i]->name[0]) != character)
		{
			character = toupper(m_dummyNodes[i]->name[0]);

			std::shared_ptr<DummyNode> textNode = std::make_shared<DummyNode>();
			textNode->textNode = true;
			textNode->name = character;
			textNode->visible = true;

			m_dummyNodes.insert(m_dummyNodes.begin() + i, textNode);
		}
	}
}

void GraphController::layoutNesting()
{
	TRACE();

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
		margins = GraphViewStyle::getMarginsOfAccessNode(node->accessKind);
	}
	else if (node->isExpandToggleNode())
	{
		margins = GraphViewStyle::getMarginsOfExpandToggleNode();
	}
	else if (node->isBundleNode())
	{
		if (node->bundledNodeType != Node::NODE_NON_INDEXED)
		{
			margins = GraphViewStyle::getMarginsForNodeType(node->bundledNodeType, false);
		}
		else
		{
			margins = GraphViewStyle::getMarginsOfBundleNode();
		}
	}
	else if (node->isQualifierNode())
	{
		return;
	}
	else if (node->isTextNode())
	{
		margins = GraphViewStyle::getMarginsOfTextNode();
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

		if (node->data->isType(Node::NODE_TYPE | Node::NODE_BUILTIN_TYPE | Node::NODE_CLASS | Node::NODE_STRUCT | Node::NODE_ENUM) &&
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
		else if (subNode->isQualifierNode())
		{
			subNode->position.y = margins.top + margins.charHeight / 2;
			width += 5;
			continue;
		}

		subNode->position.x = margins.left + x;
		subNode->position.y = margins.top + margins.charHeight + margins.spacingA + y;

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
	node->size.y = margins.top + margins.charHeight + margins.spacingA + y + height + margins.bottom;

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
	if (!node->visible || !node->isGraphNode() || !node->hasVisibleSubNode())
	{
		return;
	}

	// Increase size of nodes with visible chilren to cover full grid cells

	size_t width = GraphViewStyle::toGridSize(node->size.x);
	size_t height = GraphViewStyle::toGridSize(node->size.y);

	size_t incX = width - node->size.x;
	size_t incY = height - node->size.y;

	DummyNode* lastAccessNode = nullptr;
	DummyNode* expandToggleNode = nullptr;

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
			expandToggleNode = subNode.get();

		}
	}

	if (lastAccessNode)
	{
		lastAccessNode->size.y = lastAccessNode->size.y + incY;

		if (expandToggleNode)
		{
			expandToggleNode->position.x = expandToggleNode->position.x + incX;
		}

		node->size.x = width;
		node->size.y = height;
	}
}

void GraphController::layoutGraph(bool getSortedNodes)
{
	TRACE();

	BucketLayouter grid(getView()->getViewSize());
	grid.createBuckets(m_dummyNodes, m_dummyEdges);
	grid.layoutBuckets();

	if (getSortedNodes)
	{
		m_dummyNodes = grid.getSortedNodes();
	}
}

void GraphController::layoutList()
{
	TRACE();

	ListLayouter layouter(getView()->getViewSize());
	layouter.layoutList(m_dummyNodes);
}

void GraphController::assignBundleIds()
{
	Id bundleId = 0;
	for (size_t i = m_dummyNodes.size(); i > 0; i--)
	{
		bundleId = m_dummyNodes[i - 1]->setBundleIdRecursive(bundleId);
	}
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

void GraphController::buildGraph(MessageBase* message, bool centerActiveNode, bool animatedTransition, bool scrollToTop)
{
	if (!message->isReplayed())
	{
		GraphView::GraphParams params;
		params.centerActiveNode = centerActiveNode;
		params.animatedTransition = animatedTransition;
		params.scrollToTop = scrollToTop;
		params.isIndexedList = scrollToTop;

		getView()->rebuildGraph(m_graph, m_dummyNodes, m_dummyEdges, params);
	}
}

void GraphController::forEachDummyNodeRecursive(std::function<void(DummyNode*)> func)
{
	for (std::shared_ptr<DummyNode> node : m_dummyNodes)
	{
		node->forEachDummyNodeRecursive(func);
	}
}

void GraphController::forEachDummyEdge(std::function<void(DummyEdge*)> func)
{
	for (std::shared_ptr<DummyEdge> edge : m_dummyEdges)
	{
		func(edge.get());
	}
}

void GraphController::handleMessage(MessageColorSchemeTest* message)
{
	// todo: add nodes: package, interface, type_parameter, builtin_type
	// todo: add edges: EDGE_TYPE_ARGUMENT, EDGE_IMPORT
	// todo: add access: TYPE_PARAMETER
	clear();

	std::shared_ptr<Graph> graph = std::make_shared<Graph>();

	std::function<void(Id, Node::NodeType)> createNodes(
		[&](Id id, Node::NodeType type)
		{
			std::string name = Node::getReadableTypeString(type);
			graph->createNode(id + 1, type, NameHierarchy(name), true);
			graph->createNode(id + 2, type, NameHierarchy("focused " + name), true);
			graph->createNode(id + 3, type, NameHierarchy("active " + name), true);

			graph->createNode(id + 4, type, NameHierarchy("Non-indexed " + name), true);
			graph->createNode(id + 5, type, NameHierarchy("Non-indexed focused " + name), true);
			graph->createNode(id + 6, type, NameHierarchy("Non-indexed active " + name), true);
		}
	);

	createNodes( 0, Node::NODE_FUNCTION);
	createNodes(10, Node::NODE_GLOBAL_VARIABLE);
	createNodes(20, Node::NODE_NON_INDEXED);
	createNodes(30, Node::NODE_TYPE);
	createNodes(40, Node::NODE_TYPEDEF);
	createNodes(50, Node::NODE_NAMESPACE);
	createNodes(60, Node::NODE_FILE);
	createNodes(70, Node::NODE_MACRO);

	std::function<Node*(Node*, Id, Node::NodeType, std::string, AccessKind)> createChild(
		[&](Node* parent, Id id, Node::NodeType type, std::string name, AccessKind access)
		{
			Node* node = graph->createNode(id + 1000, type, NameHierarchy(name), true);
			graph->createEdge(id + 10000, Edge::EDGE_MEMBER, parent, node);

			if (access != ACCESS_NONE)
			{
				node->addComponentAccess(std::make_shared<TokenComponentAccess>(access));
			}

			return node;
		}
	);

	std::function<void(Id, std::string)> createEnum(
		[&](Id id, std::string name)
		{
			Node* enumNode = graph->createNode(id, Node::NODE_ENUM,
				NameHierarchy(name + Node::getReadableTypeString(Node::NODE_ENUM)), true);
			createChild(enumNode, id + 10, Node::NODE_ENUM_CONSTANT, name + Node::getReadableTypeString(Node::NODE_ENUM_CONSTANT), ACCESS_NONE);
		}
	);

	createEnum(101, "");
	createEnum(102, "focused ");
	createEnum(103, "active ");

	createEnum(104, "Non-indexed ");
	createEnum(105, "Non-indexed focused ");
	createEnum(106, "Non-indexed active ");

	std::function<void(Id, Node::NodeType, std::string)> createClass(
		[&](Id id, Node::NodeType type, std::string name)
		{
			Node* classNode = graph->createNode(id, type, NameHierarchy(name + Node::getReadableTypeString(type)), true);

			if (type == Node::NODE_CLASS)
			{
				createChild(classNode, id, Node::NODE_TEMPLATE_PARAMETER_TYPE, name + "temp_param", ACCESS_TEMPLATE_PARAMETER);
			}

			createChild(classNode, id + 10, Node::NODE_METHOD, name + "method", ACCESS_PUBLIC);
			if (type == Node::NODE_CLASS)
			{
				createChild(classNode, id + 30, Node::NODE_METHOD, name + "method", ACCESS_PROTECTED);
			}
			createChild(classNode, id + 60, Node::NODE_FIELD, name + "field", ACCESS_PRIVATE);
		}
	);

	std::function<void(Id, Node::NodeType)> createClasses(
		[&](Id id, Node::NodeType type)
		{
			createClass(id + 1, type, "");
			createClass(id + 2, type, "focused ");
			createClass(id + 3, type, "active ");

			createClass(id + 4, type, "Non-indexed ");
			createClass(id + 5, type, "Non-indexed focused ");
			createClass(id + 6, type, "Non-indexed active ");
		}
	);

	createClasses(200, Node::NODE_STRUCT);
	createClasses(300, Node::NODE_CLASS);


	std::function<void(Id, Edge::EdgeType, Node::NodeType, Node::NodeType, std::string)> createEdge(
		[&](Id id, Edge::EdgeType type, Node::NodeType origin, Node::NodeType target, std::string name)
		{
			Node* originNode;
			if (origin == Node::NODE_METHOD)
			{
				Node* classNode = graph->createNode(id + 101, Node::NODE_CLASS,
					NameHierarchy(name + Node::getReadableTypeString(Node::NODE_CLASS)), true);
				originNode = createChild(classNode, id + 111, Node::NODE_METHOD, name + Edge::getReadableTypeString(type), ACCESS_PUBLIC);
			}
			else
			{
				originNode = graph->createNode(id + 1, origin, NameHierarchy(name + Edge::getReadableTypeString(type)), true);
			}

			Node* targetNode;
			if (target == Node::NODE_METHOD)
			{
				Node* classNode = graph->createNode(id + 201, Node::NODE_CLASS,
					NameHierarchy(name + Node::getReadableTypeString(Node::NODE_CLASS)), true);
				targetNode = createChild(classNode, id + 211, Node::NODE_METHOD, name + Edge::getReadableTypeString(type), ACCESS_PUBLIC);
			}
			else
			{
				targetNode = graph->createNode(id + 11, target, NameHierarchy(name + Edge::getReadableTypeString(type)), true);
			}

			Edge* edge = graph->createEdge(id, type, originNode, targetNode);

			if (type == Edge::EDGE_AGGREGATION)
			{
				std::shared_ptr<TokenComponentAggregation> agg = std::make_shared<TokenComponentAggregation>();
				for (Id i = 1; i < 11; i++)
				{
					agg->addAggregationId(i, true);
				}
				edge->addComponentAggregation(agg);
			}
		}
	);

	std::function<void(Id, Edge::EdgeType, Node::NodeType, Node::NodeType)> createEdges(
		[&](Id id, Edge::EdgeType type, Node::NodeType origin, Node::NodeType target)
		{
			createEdge(id, type, origin, target, "");
			createEdge(id + 1000, type, origin, target, "focused ");
			createEdge(id + 2000, type, origin, target, "active ");
		}
	);

	createEdges( 100000, Edge::EDGE_CALL, Node::NODE_FUNCTION, Node::NODE_FUNCTION);
	createEdges( 200000, Edge::EDGE_USAGE, Node::NODE_GLOBAL_VARIABLE, Node::NODE_GLOBAL_VARIABLE);
	createEdges( 300000, Edge::EDGE_TYPE_USAGE, Node::NODE_FUNCTION, Node::NODE_TYPE);

	createEdges( 400000, Edge::EDGE_AGGREGATION, Node::NODE_TYPE, Node::NODE_TYPE);
	createEdges( 500000, Edge::EDGE_INCLUDE, Node::NODE_FILE, Node::NODE_FILE);
	createEdges( 600000, Edge::EDGE_MACRO_USAGE, Node::NODE_MACRO, Node::NODE_MACRO);

	createEdges( 700000, Edge::EDGE_INHERITANCE, Node::NODE_CLASS, Node::NODE_CLASS);
	createEdges( 800000, Edge::EDGE_OVERRIDE, Node::NODE_METHOD, Node::NODE_METHOD);

	createEdges( 900000, Edge::EDGE_TEMPLATE_ARGUMENT, Node::NODE_TYPE, Node::NODE_TYPE);
	createEdges(1000000, Edge::EDGE_TEMPLATE_DEFAULT_ARGUMENT, Node::NODE_TYPE, Node::NODE_TYPE);
	createEdges(1100000, Edge::EDGE_TEMPLATE_SPECIALIZATION, Node::NODE_TYPE, Node::NODE_TYPE);
	createEdges(1200000, Edge::EDGE_TEMPLATE_MEMBER_SPECIALIZATION, Node::NODE_METHOD, Node::NODE_METHOD);

	std::vector<Id> focusedTokenIds;
	std::vector<Id> activeTokenIds;

	graph->forEachNode(
		[&](Node* node)
		{
			Id id = node->getId();
			if (id % 10 < 4)
			{
				node->setExplicit(true);
			}

			if (id % 10 == 2 || id % 10 == 5)
			{
				focusedTokenIds.push_back(id);
			}
			else if (id % 10 == 3 || id % 10 == 6)
			{
				activeTokenIds.push_back(id);
			}
		}
	);

	graph->forEachEdge(
		[&](Edge* edge)
		{
			Id id = edge->getId();
			if (id % 10000 == 1000)
			{
				focusedTokenIds.push_back(id);
			}
			else if (id % 10000 == 2000)
			{
				activeTokenIds.push_back(id);
			}
		}
	);

	createDummyGraphForTokenIds(std::vector<Id>(), graph);

	for (size_t i = 0; i < 2; i++)
	{
		std::shared_ptr<DummyNode> bundleNode = std::make_shared<DummyNode>();
		bundleNode->tokenId = 10000001 + i;
		bundleNode->name = "bundle";

		if (i == 1)
		{
			bundleNode->name = "focused bundle";
			focusedTokenIds.push_back(bundleNode->tokenId);
		}

		bundleNode->bundledNodes.insert(std::make_shared<DummyNode>());
		bundleNode->bundledNodeCount = 123;
		m_dummyNodes.push_back(bundleNode);
	}

	forEachDummyNodeRecursive(
		[](DummyNode* node)
		{
			node->visible = true;

			if (node->subNodes.size())
			{
				node->expanded = true;
				node->childVisible = true;
			}
		}
	);

	forEachDummyEdge(
		[](DummyEdge* edge)
		{
			edge->visible = true;
			edge->active = true;
		}
	);

	setActive(activeTokenIds);

	layoutNesting();

	for (size_t i = 0; i < m_dummyNodes.size(); i++)
	{
		DummyNode* node = m_dummyNodes[i].get();

		node->layoutBucket.x = i % 6 + 1;
		node->layoutBucket.y = (i / 6) + 1;
	}

	BucketLayouter grid(Vec2i(0, 0));
	grid.createBuckets(m_dummyNodes, std::vector<std::shared_ptr<DummyEdge>>());
	grid.layoutBuckets();

	buildGraph(message, false);

	getView()->focusTokenIds(focusedTokenIds);
}
