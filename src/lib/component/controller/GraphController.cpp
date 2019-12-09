#include "GraphController.h"

#include <set>

#include "AccessKind.h"
#include "Application.h"
#include "ApplicationSettings.h"
#include "BucketLayouter.h"
#include "Graph.h"
#include "GraphView.h"
#include "GraphViewStyle.h"
#include "ListLayouter.h"
#include "MessageActivateNodes.h"
#include "MessageStatus.h"
#include "StorageAccess.h"
#include "TokenComponentAccess.h"
#include "TokenComponentFilePath.h"
#include "TokenComponentInheritanceChain.h"
#include "TrailLayouter.h"
#include "logging.h"
#include "tracing.h"
#include "utility.h"
#include "utilityString.h"

GraphController::GraphController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess), m_useBezierEdges(false)
{
}

Id GraphController::getSchedulerId() const
{
	return Controller::getTabId();
}

void GraphController::handleMessage(MessageActivateErrors* message)
{
	clear();
}

void GraphController::handleMessage(MessageActivateFullTextSearch* message)
{
	clear();
}

void GraphController::handleMessage(MessageActivateLegend* message)
{
	clear();

	createLegendGraph();

	layoutNesting();

	m_showsLegend = true;

	GraphView::GraphParams params;
	params.scrollToTop = true;
	buildGraph(message, params);
}

void GraphController::handleMessage(MessageActivateOverview* message)
{
	TRACE("graph all");

	clear();

	if (message->acceptedNodeTypes != NodeTypeSet::all())
	{
		createDummyGraphAndSetActiveAndVisibility(
			std::vector<Id>(),
			m_storageAccess->getGraphForNodeTypes(message->acceptedNodeTypes),
			false);

		addCharacterIndex();
		layoutNesting();
		layoutList();
	}
	else
	{
		createDummyGraphAndSetActiveAndVisibility(
			std::vector<Id>(), m_storageAccess->getGraphForAll(), false);

		bundleNodesByType();

		layoutNesting();
		assignBundleIds();
		layoutGraph();
	}

	GraphView::GraphParams params;
	params.scrollToTop = message->acceptedNodeTypes != NodeTypeSet::all();
	buildGraph(message, params);
}

void GraphController::handleMessage(MessageActivateTokens* message)
{
	TRACE("graph activate");

	if (message->isEdge || message->keepContent())
	{
		m_activeEdgeIds = message->tokenIds;
		if (message->isAggregation)	   // only on redo
		{
			m_activeEdgeIds.clear();
		}

		setActiveAndVisibility(utility::concat(m_activeNodeIds, m_activeEdgeIds));

		if (message->isReplayed())
		{
			return;
		}

		Id edgeId = 0;
		if (message->isEdge && message->tokenIds.size() == 1)
		{
			edgeId = message->tokenIds[0];
		}

		getView()->activateEdge(edgeId);
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
	std::shared_ptr<Graph> graph = m_storageAccess->getGraphForActiveTokenIds(
		tokenIds, getExpandedNodeIds(), &isNamespace);

	createDummyGraphAndSetActiveAndVisibility(tokenIds, graph, !message->isFromSearch);

	if (isNamespace)
	{
		addCharacterIndex();

		DummyNode* group = groupAllNodes(GroupType::NAMESPACE, tokenIds[0]);
		group->groupLayout = GroupLayout::LIST;

		if (!group->name.size())
		{
			group->name = m_storageAccess->getNameHierarchyForNodeId(tokenIds[0]).getQualifiedName();
			group->tokenId = tokenIds[0];
		}

		layoutNesting();
		layoutList();
	}
	else
	{
		if (m_activeNodeIds.size() == 1)
		{
			bundleNodes();
		}
		else if (message->isAggregation)
		{
			bool isInheritanceChain = true;
			for (const auto& edge: m_dummyEdges)
			{
				if (!edge->data->isType(Edge::EDGE_INHERITANCE))
				{
					isInheritanceChain = false;
					break;
				}
			}

			if (isInheritanceChain)
			{
				for (auto& node: m_dummyNodes)
				{
					node->bundleInfo.layoutVertical = true;
				}
			}

			m_useBezierEdges = !isInheritanceChain;

			for (const std::shared_ptr<DummyEdge>& edge: m_dummyEdges)
			{
				edge->active = false;
			}
		}

		groupNodesByParents(getView()->getGrouping());

		layoutNesting();
		layoutGraph(true);
		assignBundleIds();
	}

	GraphView::GraphParams params;
	params.centerActiveNode = !isNamespace;
	params.scrollToTop = isNamespace;
	buildGraph(message, params);
}

void GraphController::handleMessage(MessageActivateTrail* message)
{
	TRACE("trail activate");

	MessageStatus(L"Retrieving graph data", false, true).dispatch();

	m_activeEdgeIds.clear();

	std::shared_ptr<Graph> graph = m_storageAccess->getGraphForTrail(
		message->originId,
		message->targetId,
		message->nodeTypes,
		message->edgeTypes,
		message->nodeNonIndexed,
		message->depth,
		true /* !message->custom || (message->originId && message->targetId) */);

	// remove non-indexed files from include graph if indexed file is origin
	if (!message->custom && message->edgeTypes & Edge::EDGE_INCLUDE)
	{
		Node* fileNode = graph->getNodeById(message->originId ? message->originId : message->targetId);
		if (fileNode && fileNode->isDefined())
		{
			std::vector<Node*> nodesToRemove;
			graph->forEachNode([&nodesToRemove](Node* node) {
				if (!node->isDefined())
				{
					nodesToRemove.push_back(node);
				}
			});

			for (Node* node: nodesToRemove)
			{
				graph->removeNode(node);
			}
		}
	}

	if (message->originId && message->targetId && !graph->getNodeById(message->targetId))
	{
		MessageStatus(L"No trail graph found.", true).dispatch();

		Application::getInstance()->handleDialog(
			L"No custom trail was found between the specified symbols with the specified "
			L"parameters.",
			{L"Ok"});
	}
	else if (graph->getNodeCount() > 1000)
	{
		int r = Application::getInstance()->handleDialog(
			L"Warning!\n\nThe graph will contain " + std::to_wstring(graph->getNodeCount()) +
				" nodes. "
				L"Layouting and drawing might take a while and the resulting graph could look "
				L"confusing. Please "
				L"consider reducing graph depth with the slider on the left.\n\n"
				L"Do you want to proceed?",
			{L"Yes", L"No"});

		if (r == 1)
		{
			MessageStatus(L"Aborted graph display").dispatch();
			return;
		}
	}

	createDummyGraph(graph);
	m_graph->setTrailMode(message->horizontalLayout ? Graph::TRAIL_HORIZONTAL : Graph::TRAIL_VERTICAL);
	m_graph->setHasTrailOrigin(message->originId);

	m_activeNodeIds = {message->originId ? message->originId : message->targetId};
	setActive(m_activeNodeIds, true);
	setVisibility(true);

	MessageStatus(L"Layouting graph", false, true).dispatch();

	if (!message->custom && message->edgeTypes & Edge::EDGE_INHERITANCE)
	{
		groupTrailNodes(GroupType::INHERITANCE);
	}

	layoutNesting();
	layoutTrail(message->horizontalLayout, message->originId);

	if (message->originId && message->targetId)
	{
		DummyNode* targetNode = getDummyGraphNodeById(message->targetId).get();
		if (targetNode)
		{
			targetNode->active = true;
		}
	}

	MessageStatus(L"Displaying graph", false, true).dispatch();

	GraphView::GraphParams params;
	params.centerActiveNode = message->isLast();
	buildGraph(message, params);
}

void GraphController::handleMessage(MessageActivateTrailEdge* message)
{
	TRACE("trail edge activate");

	m_activeEdgeIds = message->edgeIds;
	setVisibility(setActive(utility::concat(m_activeNodeIds, m_activeEdgeIds), true));

	getView()->activateEdge(message->edgeIds.back());
}

void GraphController::handleMessage(MessageDeactivateEdge* message)
{
	TRACE("edge deactivate");

	m_activeEdgeIds.clear();
	setActive(utility::concat(m_activeNodeIds, m_activeEdgeIds), false);

	getView()->activateEdge(0);
}

void GraphController::handleMessage(MessageFlushUpdates* message)
{
	GraphView::GraphParams params;
	params.centerActiveNode = true;
	params.animatedTransition = !message->keepContent();
	buildGraph(message, params);
}

void GraphController::handleMessage(MessageScrollGraph* message)
{
	if (message->isReplayed())
	{
		getView()->scrollToValues(message->xValue, message->yValue);
	}
}

void GraphController::handleMessage(MessageFocusIn* message)
{
	getView()->focusTokenIds(message->tokenIds);
}

void GraphController::handleMessage(MessageFocusOut* message)
{
	getView()->defocusTokenIds(message->tokenIds);
}

void GraphController::handleMessage(MessageGraphNodeBundleSplit* message)
{
	std::wstring name;
	if (m_dummyNodes.size() == 1 && m_dummyNodes[0]->isGroupNode())
	{
		name = m_dummyNodes[0]->name;
		std::shared_ptr<DummyNode> groupNode = m_dummyNodes[0];
		m_dummyNodes = groupNode->subNodes;
	}

	for (size_t i = 0; i < m_dummyNodes.size(); i++)
	{
		DummyNode* node = m_dummyNodes[i].get();
		if ((node->isBundleNode() || node->isGroupNode()) && node->tokenId == message->bundleId)
		{
			if (!name.size())
			{
				name = node->name;
			}

			std::vector<std::shared_ptr<DummyNode>> nodes;
			if (node->isBundleNode())
			{
				nodes = std::vector<std::shared_ptr<DummyNode>>(
					node->bundledNodes.begin(), node->bundledNodes.end());
			}
			else if (node->isGroupNode())
			{
				nodes = node->subNodes;
				std::set<Id> hiddenEdgeIds(node->hiddenEdgeIds.begin(), node->hiddenEdgeIds.end());

				for (std::shared_ptr<DummyEdge>& edge: m_dummyEdges)
				{
					if (edge->ownerId == node->tokenId || edge->targetId == node->tokenId ||
						(edge->data && hiddenEdgeIds.find(edge->data->getId()) != hiddenEdgeIds.end()))
					{
						edge->hidden = false;
					}
				}

				m_topLevelAncestorIds.erase(node->tokenId);
				for (const std::shared_ptr<DummyNode>& subNode: nodes)
				{
					m_topLevelAncestorIds[subNode->tokenId] = subNode->tokenId;
				}
			}

			if (message->removeOtherNodes)
			{
				m_dummyNodes = nodes;
			}
			else
			{
				m_dummyNodes.insert(m_dummyNodes.begin() + i + 1, nodes.begin(), nodes.end());
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

	GraphView::GraphParams params;
	params.scrollToTop = message->layoutToList;
	relayoutGraph(message, params, message->layoutToList, name);
}

void GraphController::handleMessage(MessageGraphNodeExpand* message)
{
	if (message->ignoreIfNotReplayed && !message->isReplayed())
	{
		return;
	}

	if (m_graph && m_graph->getTrailMode() != Graph::TRAIL_NONE)
	{
		if (!message->isReplayed())
		{
			MessageActivateNodes(message->tokenId).dispatch();
		}
		return;
	}

	Id nodeId = message->tokenId;
	DummyNode* dummyNode = getDummyGraphNodeById(nodeId).get();
	if (dummyNode)
	{
		dummyNode->expanded = message->expand;

		if (message->expand && dummyNode->hasMissingChildNodes())
		{
			std::shared_ptr<Graph> childGraph = m_storageAccess->getGraphForChildrenOfNodeId(nodeId);

			childGraph->getNodeById(nodeId)->forEachEdgeOfType(
				Edge::EDGE_MEMBER, [this](Edge* edge) { m_graph->addEdgeAsPlainCopy(edge); });

			Node* node = m_graph->getNodeById(nodeId);
			std::vector<std::shared_ptr<DummyNode>> newDummyNodes = createDummyNodeTopDown(
				node, node->getLastParentNode()->getId());
			if (newDummyNodes.size() != 1)
			{
				LOG_ERROR("Wrong amount of dummy nodes created");
				return;
			}
			std::shared_ptr<DummyNode> newDummyNode = newDummyNodes[0];

			// replace newer dummy graph nodes with the old ones. Nodes will have the correct
			// sorting after that.
			newDummyNode->replaceSubGraphNodes(dummyNode->getSubGraphNodes());

			// move all newer and older dummy graph nodes into the old dummy node
			dummyNode->replaceAccessNodes(newDummyNode->getAccessNodes());


			if (!dummyNode->active && message->expand)
			{
				for (size_t i = 0, l = m_dummyEdges.size(); i < l; i++)
				{
					std::shared_ptr<DummyEdge> edge = m_dummyEdges[i];

					if (edge && edge->data && edge->data->isType(Edge::EDGE_AGGREGATION) &&
						(edge->targetId == dummyNode->tokenId || edge->ownerId == dummyNode->tokenId))
					{
						std::vector<Id> aggregationIds = utility::toVector<Id>(
							edge->data->getComponent<TokenComponentAggregation>()->getAggregationIds());

						if (m_graph->getEdgeById(aggregationIds[0]) != nullptr)
						{
							break;
						}

						std::shared_ptr<Graph> aggregationGraph =
							m_storageAccess->getGraphForActiveTokenIds(
								aggregationIds, std::vector<Id>());

						aggregationGraph->forEachEdge([this](Edge* e) {
							if (!e->isType(Edge::EDGE_MEMBER))
							{
								m_dummyEdges.push_back(std::make_shared<DummyEdge>(
									e->getFrom()->getId(),
									e->getTo()->getId(),
									m_graph->addEdgeAsPlainCopy(e)));
							}
						});
					}
				}
			}
		}

		setActiveAndVisibility(utility::concat(m_activeNodeIds, m_activeEdgeIds));

		layoutNesting();
		layoutGraph();

		buildGraph(message, GraphView::GraphParams());
	}
}

void GraphController::handleMessage(MessageGraphNodeHide* message)
{
	DummyNode* node = getDummyGraphNodeById(message->tokenId).get();
	DummyEdge* edge = nullptr;
	if (node)
	{
		if (node->active || node->hasActiveSubNode())
		{
			MessageStatus(L"Can't hide active node or node with active children", true).dispatch();
			return;
		}

		node->hidden = true;
	}
	else
	{
		edge = getDummyGraphEdgeById(message->tokenId);
		if (edge)
		{
			edge->hidden = true;
			edge->visible = false;

			DummyNode* from = getDummyGraphNodeById(edge->ownerId).get();
			DummyNode* to = getDummyGraphNodeById(edge->targetId).get();

			if (from)
			{
				from->connected = false;
			}

			if (to)
			{
				to->connected = false;
			}
		}
	}

	if (node || edge)
	{
		relayoutGraph(message, GraphView::GraphParams(), false, L"");
	}
}

void GraphController::handleMessage(MessageGraphNodeMove* message)
{
	DummyNode* node = getDummyGraphNodeById(message->tokenId).get();
	if (node)
	{
		node->position += message->delta;

		if (m_graph->getTrailMode() != Graph::TRAIL_NONE)
		{
			std::set<Id> childNodeIds;
			for (const std::pair<Id, Id>& p: m_topLevelAncestorIds)
			{
				if (p.second == message->tokenId)
				{
					childNodeIds.insert(p.first);
				}
			}

			for (std::shared_ptr<DummyEdge> edge: m_dummyEdges)
			{
				if (childNodeIds.find(edge->ownerId) != childNodeIds.end() ||
					childNodeIds.find(edge->targetId) != childNodeIds.end())
				{
					edge->path.clear();
				}
			}
		}

		if (message->isReplayed())
		{
			buildGraph(message, GraphView::GraphParams());
		}
		else
		{
			getView()->resizeView();
		}
	}
}

void GraphController::handleMessage(MessageShowReference* message)
{
	if (!message->tokenId || !message->fromUser)
	{
		return;
	}

	m_activeEdgeIds = std::vector<Id>(1, message->tokenId);
	setActiveAndVisibility(utility::concat(m_activeNodeIds, m_activeEdgeIds));

	GraphView::GraphParams params;
	params.animatedTransition = false;
	buildGraph(message, params);
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

	m_useBezierEdges = false;
	m_showsLegend = false;

	getView()->clear();
}

void GraphController::createDummyGraph(const std::shared_ptr<Graph> graph)
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
	m_topLevelAncestorIds.clear();

	std::set<Id> addedNodes;
	std::vector<std::shared_ptr<DummyNode>> dummyNodes;

	graph->forEachNode([&addedNodes, &dummyNodes, this](Node* node) {
		Node* parent = node->getLastParentNode();
		Id parentId = parent->getId();
		if (addedNodes.find(parentId) != addedNodes.end())
		{
			return;
		}
		addedNodes.insert(parentId);

		utility::append(dummyNodes, createDummyNodeTopDown(parent, parentId));
	});

	std::set<Id> addedEdges;
	graph->forEachEdge([&addedEdges, this](Edge* edge) {
		if (!edge->isType(Edge::EDGE_MEMBER) && addedEdges.find(edge->getId()) == addedEdges.end())
		{
			m_dummyEdges.push_back(std::make_shared<DummyEdge>(
				edge->getFrom()->getId(), edge->getTo()->getId(), edge));
			addedEdges.insert(edge->getId());
		}
	});

	updateDummyNodeNamesAndAddQualifiers(dummyNodes);

	m_dummyNodes = dummyNodes;
	m_graph = graph;

	m_useBezierEdges = false;
	m_showsLegend = false;
}

void GraphController::createDummyGraphAndSetActiveAndVisibility(
	const std::vector<Id>& tokenIds, const std::shared_ptr<Graph> graph, bool keepExpandedNodesExpanded)
{
	std::vector<Id> expandedNodeIds;

	if (keepExpandedNodesExpanded)
	{
		expandedNodeIds = getExpandedNodeIds();
	}

	createDummyGraph(graph);

	bool noActive = setActive(tokenIds, false);

	autoExpandActiveNode(tokenIds);

	if (keepExpandedNodesExpanded)
	{
		setExpandedNodeIds(expandedNodeIds);
	}

	setVisibility(noActive);

	hideBuiltinTypes();
}

std::vector<std::shared_ptr<DummyNode>> GraphController::createDummyNodeTopDown(Node* node, Id ancestorId)
{
	std::vector<std::shared_ptr<DummyNode>> nodes;

	std::shared_ptr<DummyNode> result = std::make_shared<DummyNode>(DummyNode::DUMMY_DATA);
	result->data = node;
	result->name = node->getName();

	result->tokenId = node->getId();
	m_topLevelAncestorIds.emplace(node->getId(), ancestorId);

	m_dummyGraphNodes.emplace(result->data->getId(), result);
	nodes.push_back(result);

	if (node->getType().isPackage())
	{
		node->forEachChildNode([&nodes, &ancestorId, this](Node* child) {
			utility::append(nodes, createDummyNodeTopDown(child, ancestorId));
		});

		return nodes;
	}

	node->forEachChildNode([&result, &ancestorId, this](Node* child) {
		DummyNode* parent = nullptr;
		AccessKind accessKind = ACCESS_NONE;

		TokenComponentAccess* access = child->getComponent<TokenComponentAccess>();
		if (access)
		{
			accessKind = access->getAccess();
		}

		for (const std::shared_ptr<DummyNode>& dummy: result->subNodes)
		{
			if (dummy->accessKind == accessKind)
			{
				parent = dummy.get();
				break;
			}
		}

		if (!parent)
		{
			std::shared_ptr<DummyNode> accessNode = std::make_shared<DummyNode>(
				DummyNode::DUMMY_ACCESS);
			accessNode->accessKind = accessKind;
			result->subNodes.push_back(accessNode);
			parent = accessNode.get();
		}

		utility::append(parent->subNodes, createDummyNodeTopDown(child, ancestorId));
	});

	return nodes;
}

void GraphController::updateDummyNodeNamesAndAddQualifiers(
	const std::vector<std::shared_ptr<DummyNode>>& dummyNodes)
{
	for (const std::shared_ptr<DummyNode>& node: dummyNodes)
	{
		if (node->isGroupNode() || !node->data || node->data->getType().isFile())
		{
			updateDummyNodeNamesAndAddQualifiers(node->subNodes);
		}
		else if (node->data->getType().isPackage())
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
				std::shared_ptr<DummyNode> qualifierNode = std::make_shared<DummyNode>(
					DummyNode::DUMMY_QUALIFIER);
				qualifierNode->qualifierName = qualifier;
				qualifierNode->visible = true;

				node->subNodes.push_back(qualifierNode);
				node->qualifierName = qualifier;
			}
		}
	}
}

std::vector<Id> GraphController::getExpandedNodeIds() const
{
	std::vector<Id> nodeIds;
	for (const std::pair<Id, std::shared_ptr<DummyNode>>& p: m_dummyGraphNodes)
	{
		DummyNode* oldNode = p.second.get();
		if (oldNode->expanded && !oldNode->autoExpanded && oldNode->isGraphNode() &&
			!oldNode->data->isType(NodeType::NODE_FUNCTION | NodeType::NODE_METHOD))
		{
			nodeIds.push_back(p.first);
		}
	}
	return nodeIds;
}

void GraphController::setExpandedNodeIds(const std::vector<Id>& nodeIds)
{
	for (Id id: nodeIds)
	{
		DummyNode* node = getDummyGraphNodeById(id).get();
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
		node = getDummyGraphNodeById(activeTokenIds[0]).get();
	}

	if (node && !node->hasMissingChildNodes())
	{
		node->expanded = true;
		node->autoExpanded = true;
	}
}

bool GraphController::setActive(const std::vector<Id>& activeTokenIds, bool showAllEdges)
{
	TRACE();

	bool noActive = !activeTokenIds.size();
	if (activeTokenIds.size() > 0)
	{
		noActive = true;
		for (const std::shared_ptr<DummyNode>& node: m_dummyNodes)
		{
			if (setNodeActiveRecursive(node.get(), activeTokenIds))
			{
				noActive = false;
			}
		}
	}

	bool noActiveFinal = noActive;
	for (const std::shared_ptr<DummyEdge>& edge: m_dummyEdges)
	{
		if (!edge->data)
		{
			continue;
		}

		edge->active = false;
		if (find(activeTokenIds.begin(), activeTokenIds.end(), edge->data->getId()) !=
			activeTokenIds.end())
		{
			edge->active = true;
			noActiveFinal = false;
		}

		DummyNode* from = getDummyGraphNodeById(edge->ownerId).get();
		DummyNode* to = getDummyGraphNodeById(edge->targetId).get();

		bool isInheritance = edge->data->isType(Edge::EDGE_INHERITANCE);
		if (from && to && !edge->hidden &&
			(showAllEdges || noActive || from->active || to->active || edge->active || isInheritance) &&
			!(to->active && edge->data->isType(Edge::EDGE_TYPE_USAGE) &&
			  to->data->isParentOf(from->data)))	// Don't show type use edges to active parent
		{
			edge->visible = true;
			from->connected = true;
			to->connected = true;
		}
		else
		{
			edge->visible = false;
		}
	}

	return noActiveFinal;
}

void GraphController::setVisibility(bool noActive)
{
	TRACE();

	for (const std::shared_ptr<DummyNode>& node: m_dummyNodes)
	{
		setNodeVisibilityRecursiveBottomUp(node.get(), noActive);
	}
}

void GraphController::setActiveAndVisibility(const std::vector<Id>& activeTokenIds)
{
	TRACE();

	setVisibility(setActive(activeTokenIds, false));
}

bool GraphController::setNodeActiveRecursive(DummyNode* node, const std::vector<Id>& activeTokenIds) const
{
	bool hasActive = false;
	node->active = false;

	if (node->isGraphNode())
	{
		node->active = find(activeTokenIds.begin(), activeTokenIds.end(), node->data->getId()) !=
			activeTokenIds.end();

		if (node->active)
		{
			hasActive = true;
		}
	}

	for (const std::shared_ptr<DummyNode>& subNode: node->subNodes)
	{
		if (setNodeActiveRecursive(subNode.get(), activeTokenIds))
		{
			hasActive = true;
		}
	}

	return hasActive;
}

bool GraphController::setNodeVisibilityRecursiveBottomUp(DummyNode* node, bool noActive) const
{
	node->visible = false;
	node->childVisible = false;

	if (node->hidden)
	{
		return false;
	}
	else if (node->isExpandToggleNode())
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

	for (const std::shared_ptr<DummyNode>& subNode: node->subNodes)
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
	if (node->isGraphNode() && node->data->getType().getType() == NodeType::NODE_ENUM &&
		!node->isExpanded())
	{
		node->visible = true;
		return;
	}

	if ((node->isGraphNode() && node->isExpanded()) ||
		(node->isAccessNode() && (node->accessKind == ACCESS_NONE || parentExpanded)))
	{
		for (const std::shared_ptr<DummyNode>& subNode: node->subNodes)
		{
			if (!subNode->isQualifierNode() && !subNode->isExpandToggleNode() && !subNode->hidden)
			{
				setNodeVisibilityRecursiveTopDown(subNode.get(), node->isExpanded());
				node->childVisible |= subNode->visible;
			}
		}
	}

	if (!node->isAccessNode() || node->childVisible)
	{
		node->visible = true;
	}
}

void GraphController::hideBuiltinTypes()
{
	if (ApplicationSettings::getInstance()->getShowBuiltinTypesInGraph() ||
		m_activeNodeIds.size() != 1)
	{
		return;
	}

	for (const std::shared_ptr<DummyNode>& node: m_dummyNodes)
	{
		if (node->isGraphNode() && !node->active && node->data->getType().isBuiltin())
		{
			node->visible = false;
			node->hidden = true;
		}
	}
}

void GraphController::bundleNodes()
{
	TRACE();

	// evaluate top level nodes
	for (const std::shared_ptr<DummyNode>& node: m_dummyNodes)
	{
		if (!node->isGraphNode() || !node->visible)
		{
			continue;
		}

		DummyNode::BundleInfo* bundleInfo = &node->bundleInfo;
		bundleInfo->isActive = node->hasActiveSubNode();

		node->data->forEachNodeRecursive([&bundleInfo](const Node* n) {
			if (n->isDefined())
			{
				bundleInfo->isDefined = true;
			}

			if (bundleInfo->layoutVertical)
			{
				return;
			}

			n->forEachEdgeOfType(~Edge::EDGE_MEMBER, [&bundleInfo, &n](Edge* e) {
				if (bundleInfo->layoutVertical)
				{
					return;
				}

				if (e->isType(Edge::LAYOUT_VERTICAL))
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
					else if (
						(dir == TokenComponentAggregation::DIRECTION_FORWARD && e->getFrom() == n) ||
						(dir == TokenComponentAggregation::DIRECTION_BACKWARD && e->getTo() == n))
					{
						bundleInfo->isReferencing = true;
					}
					else if (
						(dir == TokenComponentAggregation::DIRECTION_FORWARD && e->getTo() == n) ||
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
			});
		});

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
	// 	std::wcout << node->name << std::endl;
	// }

	// bundle
	bool fileOrMacroActive = false;
	for (const std::shared_ptr<DummyNode>& node: m_dummyNodes)
	{
		if (node->bundleInfo.isActive &&
			(node->data->isType(NodeType::NODE_FILE | NodeType::NODE_MACRO) ||
			 node->data->findEdgeOfType(Edge::EDGE_INCLUDE | Edge::EDGE_MACRO_USAGE) != nullptr))
		{
			fileOrMacroActive = true;
			break;
		}
	}

	if (fileOrMacroActive)
	{
		return;
	}

	bundleNodesAndEdgesMatching(
		[](const DummyNode::BundleInfo& info, const Node* data) {
			return data->getType().isFile() && data->findEdgeOfType(Edge::EDGE_IMPORT);
		},
		1,
		false,
		L"Importing Files");

	bundleNodesAndEdgesMatching(
		[](const DummyNode::BundleInfo& info, const Node* data) {
			return !info.isDefined && info.isReferencing && !info.layoutVertical;
		},
		2,
		true,
		L"Non-indexed Symbols");

	bundleNodesAndEdgesMatching(
		[](const DummyNode::BundleInfo& info, const Node* data) {
			return !info.isDefined && info.isReferenced && !info.layoutVertical;
		},
		2,
		true,
		L"Non-indexed Symbols");

	bundleNodesAndEdgesMatching(
		[](const DummyNode::BundleInfo& info, const Node* data) {
			return info.isDefined && info.isReferenced && data->getType().isBuiltin();
		},
		3,
		false,
		L"Built-in Types");

	bundleNodesAndEdgesMatching(
		[](const DummyNode::BundleInfo& info, const Node* data) {
			return info.isDefined && info.isReferencing && !info.layoutVertical;
		},
		10,
		false,
		L"Referencing Symbols");

	bundleNodesAndEdgesMatching(
		[](const DummyNode::BundleInfo& info, const Node* data) {
			return info.isDefined && info.isReferenced && !info.layoutVertical;
		},
		10,
		false,
		L"Referenced Symbols");

	bundleNodesAndEdgesMatching(
		[](const DummyNode::BundleInfo& info, const Node* data) {
			return info.isReferencing && info.layoutVertical &&
				data->findEdgeOfType(Edge::EDGE_TEMPLATE_SPECIALIZATION);
		},
		5,
		false,
		L"Specializing Symbols");

	bundleNodesAndEdgesMatching(
		[](const DummyNode::BundleInfo& info, const Node* data) {
			return info.isReferencing && info.layoutVertical &&
				data->findEdgeOfType(Edge::EDGE_INHERITANCE);
		},
		5,
		false,
		L"Derived Symbols");

	bundleNodesAndEdgesMatching(
		[](const DummyNode::BundleInfo& info, const Node* data) {
			return info.isReferenced && info.layoutVertical &&
				data->findEdgeOfType(Edge::EDGE_INHERITANCE);
		},
		5,
		false,
		L"Base Symbols");
}

void GraphController::bundleNodesAndEdgesMatching(
	std::function<bool(const DummyNode::BundleInfo&, const Node* data)> matcher,
	size_t count,
	bool countConnectedNodes,
	const std::wstring& name)
{
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
	if (!matchedNodeIndices.size() || matchedNodeCount < count ||
		matchedNodeIndices.size() == m_dummyNodes.size())
	{
		return;
	}

	std::shared_ptr<DummyNode> bundleNode = std::make_shared<DummyNode>(DummyNode::DUMMY_BUNDLE);
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
	bundleNode->tokenId = ~(~Id(0) >> 1) + firstNode->data->getId();
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
	for (const DummyNode* node: bundledNodes)
	{
		for (const std::shared_ptr<DummyEdge>& edge: m_dummyEdges)
		{
			bool owner = (edge->ownerId == node->data->getId());
			bool target = (edge->targetId == node->data->getId());

			if (!owner && !target)
			{
				continue;
			}

			DummyEdge* bundleEdgePtr = nullptr;
			for (const std::shared_ptr<DummyEdge>& bundleEdge: bundleEdges)
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
	std::list<std::shared_ptr<DummyNode>>& nodes,
	std::function<bool(const DummyNode*)> matcher,
	const std::wstring& name)
{
	std::vector<std::list<std::shared_ptr<DummyNode>>::iterator> matchedNodes;
	for (std::list<std::shared_ptr<DummyNode>>::iterator it = nodes.begin(); it != nodes.end(); it++)
	{
		if (matcher(it->get()))
		{
			matchedNodes.push_back(it);
		}
	}

	if (matchedNodes.empty())
	{
		return nullptr;
	}

	std::shared_ptr<DummyNode> bundleNode = std::make_shared<DummyNode>(DummyNode::DUMMY_BUNDLE);
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
	bundleNode->tokenId = ~(~Id(0) >> 1) + (*bundleNode->bundledNodes.begin())->data->getId();
	return bundleNode;
}

std::shared_ptr<DummyNode> GraphController::bundleByType(
	std::list<std::shared_ptr<DummyNode>>& nodes,
	const NodeType& type,
	const Tree<NodeType::BundleInfo>& bundleInfoTree,
	const bool considerInvisibleNodes)
{
	std::shared_ptr<DummyNode> bundleNode = bundleNodesMatching(
		nodes,
		[&](const DummyNode* node) {
			return (considerInvisibleNodes || node->visible) && node->isGraphNode() &&
				node->data->getType() == type && bundleInfoTree.data.nameMatcher(node->name);
		},
		bundleInfoTree.data.bundleName);

	if (bundleNode)
	{
		bundleNode->bundledNodeType = type;
		bundleNode->bundledNodeCount = bundleNode->getBundledNodeCount();

		if (!bundleInfoTree.children.empty())
		{
			std::list<std::shared_ptr<DummyNode>> bundledNodes(
				bundleNode->bundledNodes.begin(), bundleNode->bundledNodes.end());
			bundleNode->bundledNodes.clear();

			// crate a sub-bundle for anonymous namespaces
			for (const Tree<NodeType::BundleInfo>& childBundleInfoTree: bundleInfoTree.children)
			{
				std::shared_ptr<DummyNode> childBundle = bundleByType(
					bundledNodes, type, childBundleInfoTree, true);
				if (childBundle)
				{
					bundleNode->bundledNodes.insert(childBundle);
				}
			}

			bundleNode->bundledNodes.insert(bundledNodes.begin(), bundledNodes.end());
		}
	}

	return bundleNode;
}

void GraphController::bundleNodesByType()
{
	TRACE();

	std::list<std::shared_ptr<DummyNode>> nodes(m_dummyNodes.begin(), m_dummyNodes.end());
	std::vector<std::shared_ptr<DummyNode>> oldNodes = std::move(m_dummyNodes);
	m_dummyNodes.clear();

	bool hasNonFileBundle = false;

	for (const NodeType& nodeType: NodeType::getOverviewBundleNodeTypesOrdered())
	{
		Tree<NodeType::BundleInfo> bundleInfoTree = nodeType.getOverviewBundleTree();
		if (bundleInfoTree.data.isValid())
		{
			std::shared_ptr<DummyNode> bundleNode = bundleByType(
				nodes, nodeType, bundleInfoTree, false);
			if (bundleNode)
			{
				m_dummyNodes.push_back(bundleNode);

				if (bundleNode->bundledNodeType.getType() != NodeType::NODE_FILE)
				{
					hasNonFileBundle = true;
				}
			}
		}
	}

	if (nodes.size() && !hasNonFileBundle)
	{
		Tree<NodeType::BundleInfo> bundleInfoTree(NodeType::BundleInfo(L"Symbols"));
		std::shared_ptr<DummyNode> bundleNode = bundleByType(
			nodes, NodeType::NODE_SYMBOL, bundleInfoTree, false);
		if (bundleNode)
		{
			m_dummyNodes.push_back(bundleNode);
		}
	}

	if (nodes.size())
	{
		LOG_ERROR("Nodes left after bundling for overview");
	}
}

void GraphController::addCharacterIndex()
{
	// Remove index characters from last time
	DummyNode::BundledNodesSet newNodes;
	for (const std::shared_ptr<DummyNode>& node: m_dummyNodes)
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
		if (!m_dummyNodes[i]->visible || !m_dummyNodes[i]->name.size())
		{
			continue;
		}

		if (towupper(m_dummyNodes[i]->name[0]) != character)
		{
			character = towupper(m_dummyNodes[i]->name[0]);

			std::shared_ptr<DummyNode> textNode = std::make_shared<DummyNode>(DummyNode::DUMMY_TEXT);
			textNode->name = character;
			textNode->visible = true;

			m_dummyNodes.insert(m_dummyNodes.begin() + i, textNode);
		}
	}
}

bool GraphController::hasCharacterIndex() const
{
	for (const std::shared_ptr<DummyNode>& node: m_dummyNodes)
	{
		if (node->isTextNode())
		{
			return true;
		}
	}
	return false;
}

void GraphController::groupNodesByParents(GroupType groupType)
{
	TRACE();

	if (groupType != GroupType::FILE && groupType != GroupType::NAMESPACE)
	{
		return;
	}

	std::map<std::wstring, std::shared_ptr<DummyNode>> groupNodes;
	std::map<std::wstring, std::vector<std::shared_ptr<DummyNode>>> nodesToGroup;

	std::map<Id, std::pair<Id, NameHierarchy>> nodeIdtoParentMap;
	if (groupType == GroupType::FILE)
	{
		std::vector<Id> nodeIds;
		for (const std::shared_ptr<DummyNode>& dummyNode: m_dummyNodes)
		{
			if (dummyNode->isGraphNode())
			{
				nodeIds.push_back(dummyNode->tokenId);
			}
		}

		nodeIdtoParentMap = m_storageAccess->getNodeIdToParentFileMap(nodeIds);
	}

	std::map<std::wstring, Id> qualifierNameToIdMap;
	for (const std::shared_ptr<DummyNode>& dummyNode: m_dummyNodes)
	{
		if (dummyNode->isGroupNode())
		{
			groupNodes.emplace(dummyNode->name, dummyNode);
		}
		else if (dummyNode->visible)
		{
			if (groupType == GroupType::FILE)
			{
				if (dummyNode->isGraphNode())
				{
					auto it = nodeIdtoParentMap.find(dummyNode->tokenId);
					if (it != nodeIdtoParentMap.end())
					{
						nodesToGroup[it->second.second.getQualifiedName()].push_back(dummyNode);
					}
				}
			}
			else if (groupType == GroupType::NAMESPACE)
			{
				const DummyNode* qualifierNode = dummyNode->getQualifierNode();
				if (qualifierNode)
				{
					Id qualifierId = 0;
					std::wstring qualifierName = qualifierNode->qualifierName.getQualifiedName();
					auto it = qualifierNameToIdMap.find(qualifierName);
					if (it != qualifierNameToIdMap.end())
					{
						qualifierId = it->second;
					}
					else
					{
						qualifierId = m_storageAccess->getNodeIdForNameHierarchy(
							qualifierNode->qualifierName);
						qualifierNameToIdMap.emplace(qualifierName, qualifierId);
					}

					nodesToGroup[qualifierName].push_back(dummyNode);
					nodeIdtoParentMap.emplace(
						dummyNode->tokenId,
						std::make_pair(qualifierId, qualifierNode->qualifierName));
				}
			}
		}
	}

	std::set<Id> groupedNodeIds;
	for (const std::pair<std::wstring, std::vector<std::shared_ptr<DummyNode>>>& p: nodesToGroup)
	{
		std::shared_ptr<DummyNode> groupNode;

		std::wstring name = p.first;
		if (groupType == GroupType::FILE)
		{
			name = FilePath(p.first).fileName();
		}

		auto it = groupNodes.find(name);
		if (it != groupNodes.end())
		{
			groupNode = it->second;
		}
		else
		{
			groupNode = std::make_shared<DummyNode>(DummyNode::DUMMY_GROUP);
			groupNode->visible = true;
			groupNode->groupType = groupType;
			groupNode->groupLayout = GroupLayout::BUCKET;
			groupNode->name = name;

			auto it = nodeIdtoParentMap.find(p.second[0]->tokenId);
			if (it != nodeIdtoParentMap.end())
			{
				groupNode->tokenId = it->second.first;
			}
			m_topLevelAncestorIds[groupNode->tokenId] = groupNode->tokenId;
			m_dummyNodes.push_back(groupNode);
		}

		for (std::shared_ptr<DummyNode> dummyNode: p.second)
		{
			if (dummyNode->hasActiveSubNode())
			{
				groupNode->bundleInfo = dummyNode->bundleInfo;
				groupNode->bundleId = dummyNode->bundleId;
			}

			groupNode->subNodes.push_back(dummyNode);
			m_topLevelAncestorIds[dummyNode->tokenId] = groupNode->tokenId;
			groupedNodeIds.insert(dummyNode->tokenId);
		}

		if (!groupNode->bundleId)
		{
			groupNode->bundleId = groupNode->subNodes[0]->bundleId;
		}

		groupNode->bundleInfo = DummyNode::BundleInfo::averageBundleInfo(groupNode->getBundleInfos());
		groupNode->sortSubNodesByName();
	}

	for (int i = 0; i < int(m_dummyNodes.size()); i++)
	{
		if (groupedNodeIds.find(m_dummyNodes[i]->tokenId) != groupedNodeIds.end())
		{
			m_dummyNodes.erase(m_dummyNodes.begin() + i);
			i--;
		}
	}
}

DummyNode* GraphController::groupAllNodes(GroupType groupType, Id groupNodeId)
{
	TRACE();

	std::shared_ptr<DummyNode> groupNode = std::make_shared<DummyNode>(DummyNode::DUMMY_GROUP);
	groupNode->visible = true;
	groupNode->groupType = groupType;
	groupNode->tokenId = groupNodeId;
	m_topLevelAncestorIds[groupNode->tokenId] = groupNode->tokenId;

	for (std::shared_ptr<DummyNode> dummyNode: m_dummyNodes)
	{
		groupNode->subNodes.push_back(dummyNode);
		m_topLevelAncestorIds[dummyNode->tokenId] = groupNode->tokenId;
	}

	if (groupNode->subNodes.size())
	{
		m_dummyNodes = {groupNode};
	}

	return groupNode.get();
}

void GraphController::groupTrailNodes(GroupType groupType)
{
	TRACE();

	struct TrailNode
	{
		Id nodeId;
		std::set<Id> targetNodeIds;
		std::set<Id> originNodeIds;

		std::vector<DummyEdge*> targetEdges;
		std::vector<DummyEdge*> originEdges;
	};

	std::set<Id> possibleNodeIds;
	for (auto dummyNode: m_dummyNodes)
	{
		if (dummyNode->visible && dummyNode->tokenId &&
			(!dummyNode->subNodes.size() ||
			 (dummyNode->subNodes.size() == 1 && dummyNode->subNodes[0]->isQualifierNode())))
		{
			possibleNodeIds.insert(dummyNode->tokenId);
		}
	}

	std::map<Id, TrailNode> nodes;

	for (const std::shared_ptr<DummyEdge>& edge: m_dummyEdges)
	{
		if (edge->visible && possibleNodeIds.find(edge->ownerId) != possibleNodeIds.end() &&
			possibleNodeIds.find(edge->targetId) != possibleNodeIds.end())
		{
			TrailNode& fromNode = nodes[edge->ownerId];
			fromNode.nodeId = edge->ownerId;
			fromNode.targetNodeIds.insert(edge->targetId);
			fromNode.targetEdges.push_back(edge.get());

			TrailNode& toNode = nodes[edge->targetId];
			toNode.nodeId = edge->targetId;
			toNode.originNodeIds.insert(edge->ownerId);
			toNode.originEdges.push_back(edge.get());
		}
	}

	std::set<Id> groupedNodeIds;
	while (nodes.size())
	{
		TrailNode node = nodes.begin()->second;
		nodes.erase(nodes.begin());

		std::vector<TrailNode> group;
		std::map<Id, TrailNode>::iterator it = nodes.begin();
		while (it != nodes.end())
		{
			if (node.targetNodeIds.size() <= 1 && it->second.targetNodeIds == node.targetNodeIds &&
				node.originNodeIds.size() <= 1 && it->second.originNodeIds == node.originNodeIds)
			{
				group.push_back(it->second);
				it = nodes.erase(it);
			}
			else
			{
				it++;
			}
		}

		group.push_back(node);
		if (group.size() < 3)
		{
			continue;
		}

		std::shared_ptr<DummyNode> groupNode = std::make_shared<DummyNode>(DummyNode::DUMMY_GROUP);
		groupNode->visible = true;
		groupNode->groupType = groupType;
		groupNode->groupLayout = GroupLayout::SQUARE;

		// Use token Id of first node and make first 2 bits 1
		groupNode->tokenId = ~(~Id(0) >> 2) + node.nodeId;
		m_topLevelAncestorIds[groupNode->tokenId] = groupNode->tokenId;

		std::shared_ptr<DummyEdge> targetEdge = std::make_shared<DummyEdge>();
		targetEdge->ownerId = groupNode->tokenId;

		std::shared_ptr<DummyEdge> originEdge = std::make_shared<DummyEdge>();
		originEdge->targetId = groupNode->tokenId;

		std::vector<Id> hiddenEdgeIds;

		for (TrailNode& node: group)
		{
			std::shared_ptr<DummyNode> dummyNode = getDummyGraphNodeById(node.nodeId);
			if (!dummyNode)
			{
				continue;
			}

			groupedNodeIds.insert(node.nodeId);
			groupNode->subNodes.push_back(dummyNode);

			m_topLevelAncestorIds[node.nodeId] = groupNode->tokenId;

			for (DummyEdge* edge: node.targetEdges)
			{
				if (!targetEdge->visible)
				{
					targetEdge->visible = true;
					targetEdge->targetId = edge->targetId;
					targetEdge->data = edge->data;
				}

				edge->visible = false;
				edge->hidden = true;

				if (edge->data)
				{
					groupNode->hiddenEdgeIds.push_back(edge->data->getId());
				}
			}

			for (DummyEdge* edge: node.originEdges)
			{
				if (!originEdge->visible)
				{
					originEdge->visible = true;
					originEdge->ownerId = edge->ownerId;
					originEdge->data = edge->data;
				}

				edge->visible = false;
				edge->hidden = true;

				if (edge->data)
				{
					groupNode->hiddenEdgeIds.push_back(edge->data->getId());
				}
			}
		}

		if (targetEdge->visible)
		{
			m_dummyEdges.push_back(targetEdge);
		}

		if (originEdge->visible)
		{
			m_dummyEdges.push_back(originEdge);
		}

		groupNode->sortSubNodesByName();
		m_dummyNodes.push_back(groupNode);
	}

	for (int i = 0; i < int(m_dummyNodes.size()); i++)
	{
		if (groupedNodeIds.find(m_dummyNodes[i]->tokenId) != groupedNodeIds.end())
		{
			m_dummyNodes.erase(m_dummyNodes.begin() + i);
			i--;
		}
	}
}

void GraphController::layoutNesting()
{
	TRACE();

	extendEqualFunctionNames(m_dummyNodes);

	for (const std::shared_ptr<DummyNode>& node: m_dummyNodes)
	{
		layoutNestingRecursive(node.get());
	}

	for (const std::shared_ptr<DummyNode>& node: m_dummyNodes)
	{
		layoutToGrid(node.get());
	}
}

void GraphController::extendEqualFunctionNames(const std::vector<std::shared_ptr<DummyNode>>& nodes) const
{
	std::multimap<std::wstring, std::shared_ptr<DummyNode>> functionNames;
	for (auto& node: nodes)
	{
		if (node->visible && node->isGraphNode() &&
			node->data->isType(NodeType::NODE_FUNCTION | NodeType::NODE_METHOD))
		{
			functionNames.emplace(node->name, node);
		}
	}

	for (auto it: functionNames)
	{
		if (functionNames.count(it.first) < 2)
		{
			continue;
		}

		auto ret = functionNames.equal_range(it.first);
		for (auto it2 = ret.first; it2 != ret.second; it2++)
		{
			it2->second->name =
				it2->second->data->getNameHierarchy().getRawNameWithSignatureParameters();
		}
	}

	for (auto& node: nodes)
	{
		if (node->subNodes.size())
		{
			extendEqualFunctionNames(node->subNodes);
		}
	}
}

Vec4i GraphController::layoutNestingRecursive(DummyNode* node, int relayoutAccessMaxWidth) const
{
	if (!node->visible)
	{
		return Vec4i(0, 0, 0, 0);
	}

	GraphViewStyle::NodeMargins margins;

	if (node->isGraphNode())
	{
		margins = GraphViewStyle::getMarginsForDataNode(
			node->data->getType().getNodeStyle(), node->data->getType().hasIcon(), node->childVisible);
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
		if (node->bundledNodeType.getType() != NodeType::NODE_SYMBOL)
		{
			margins = GraphViewStyle::getMarginsForDataNode(
				node->bundledNodeType.getNodeStyle(), node->bundledNodeType.hasIcon(), false);
		}
		else
		{
			margins = GraphViewStyle::getMarginsOfBundleNode();
		}
	}
	else if (node->isQualifierNode())
	{
		return Vec4i(0, 0, 0, 0);
	}
	else if (node->isTextNode())
	{
		margins = GraphViewStyle::getMarginsOfTextNode(node->fontSizeDiff);
	}
	else if (node->isGroupNode())
	{
		margins = GraphViewStyle::getMarginsOfGroupNode(node->groupType, node->name.size());
	}

	int width = 0;
	int height = 0;

	if (node->isGraphNode())
	{
		node->name = utility::elide(node->name, utility::ELIDE_RIGHT, node->active ? 100 : 50);
		width = margins.charWidth * node->name.size();

		if (node->data->getType().isCollapsible() && node->data->getChildCount() > 0)
		{
			addExpandToggleNode(node);
		}
	}
	else if (node->isBundleNode() || node->isTextNode())
	{
		width = margins.charWidth * node->name.size();
	}
	else if (node->isGroupNode())
	{
		width = margins.charWidth * node->name.size() + 5;
	}

	width += margins.iconWidth;
	width = std::max(width, margins.minWidth);

	if (relayoutAccessMaxWidth == -1)
	{
		int maxAccessWidth = 0;
		std::shared_ptr<const DummyNode> maxWidthAccessNode;

		for (const std::shared_ptr<DummyNode>& subNode: node->subNodes)
		{
			if (!subNode->visible)
			{
				continue;
			}
			else if (subNode->isQualifierNode())
			{
				subNode->position.y = margins.top + margins.charHeight / 2;
				width += 5;
				continue;
			}

			Vec4i rect = layoutNestingRecursive(subNode.get());

			if (subNode->isExpandToggleNode())
			{
				width += margins.spacingX + subNode->size.x;
			}
			else if (subNode->isAccessNode() && rect.z() > maxAccessWidth)
			{
				maxAccessWidth = rect.z();
				maxWidthAccessNode = subNode;
			}
		}

		if (maxAccessWidth > 0)
		{
			for (const std::shared_ptr<DummyNode>& subNode: node->subNodes)
			{
				if (subNode->visible && subNode->isAccessNode() && subNode != maxWidthAccessNode)
				{
					layoutNestingRecursive(subNode.get(), maxAccessWidth);
				}
			}
		}
	}

	if (node->subNodes.size())
	{
		if (node->isGroupNode())
		{
			Vec2i viewSize = getView()->getViewSize();

			switch (node->groupLayout)
			{
			case GroupLayout::LIST:
				viewSize.x = viewSize.x - 150;	  // prevent horizontal scroll
				ListLayouter::layoutMultiColumn(viewSize, &node->subNodes);
				break;

			case GroupLayout::SKEWED:
				ListLayouter::layoutSkewed(
					&node->subNodes, margins.spacingX, margins.spacingY, viewSize.x() * 1.5);
				break;

			case GroupLayout::BUCKET:
				if (node->hasActiveSubNode() || !m_activeNodeIds.size() /* aggregations */)
				{
					BucketLayouter grid(viewSize);
					grid.createBuckets(node->subNodes, m_dummyEdges);
					grid.layoutBuckets(m_activeNodeIds.size());
					node->subNodes = grid.getSortedNodes();
				}
				else
				{
					ListLayouter::layoutColumn(&node->subNodes, margins.spacingY);
				}
				break;

			case GroupLayout::SQUARE:
				ListLayouter::layoutSquare(&node->subNodes, -1);
				break;
			}
		}
		else if (node->isAccessNode() && !node->hasConnectedSubNode())
		{
			ListLayouter::layoutSquare(&node->subNodes, relayoutAccessMaxWidth);
		}
		else
		{
			ListLayouter::layoutColumn(&node->subNodes, margins.spacingY);
		}
	}

	Vec2i size = ListLayouter::offsetNodes(
		node->subNodes, margins.top + margins.charHeight + margins.spacingA, margins.left);

	width = std::max(size.x(), width);
	height = size.y();

	node->size.x = margins.left + width + margins.right;
	node->size.y = margins.top + margins.charHeight + margins.spacingA + height + margins.bottom;

	for (const std::shared_ptr<DummyNode>& subNode: node->subNodes)
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

	return ListLayouter::boundingRect(node->subNodes);
}

void GraphController::addExpandToggleNode(DummyNode* node) const
{
	std::shared_ptr<DummyNode> expandNode = std::make_shared<DummyNode>(
		DummyNode::DUMMY_EXPAND_TOGGLE);
	expandNode->expanded = node->expanded;
	expandNode->visible = true;

	size_t visibleSubNodeCount = 0;
	for (size_t i = 0; i < node->subNodes.size(); i++)
	{
		DummyNode* subNode = node->subNodes[i].get();

		if (subNode->isExpandToggleNode())
		{
			node->subNodes.erase(node->subNodes.begin() + i);
			i--;
			continue;
		}

		if (subNode->isQualifierNode())
		{
			continue;
		}

		for (const std::shared_ptr<DummyNode>& subSubNode: subNode->subNodes)
		{
			if ((subSubNode->visible || subSubNode->hidden) &&
				(!subSubNode->isGraphNode() || !subSubNode->data->isImplicit() ||
				 node->data->isImplicit()))
			{
				visibleSubNodeCount++;
			}
		}
	}

	expandNode->invisibleSubNodeCount = node->data->getChildCount() - visibleSubNodeCount;
	if ((expandNode->isExpanded() && visibleSubNodeCount > 0) || expandNode->invisibleSubNodeCount)
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

	for (const std::shared_ptr<DummyNode>& subNode: node->subNodes)
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

	std::vector<std::shared_ptr<DummyNode>> visibleNodes;
	for (auto node: m_dummyNodes)
	{
		if (node->visible)
		{
			visibleNodes.push_back(node);
		}
	}

	BucketLayouter grid(getView()->getViewSize());
	grid.createBuckets(visibleNodes, m_dummyEdges);
	grid.layoutBuckets(false);

	if (getSortedNodes)
	{
		m_dummyNodes = grid.getSortedNodes();
	}
}

void GraphController::layoutList()
{
	TRACE();

	ListLayouter::layoutMultiColumn(getView()->getViewSize(), &m_dummyNodes);
}

void GraphController::layoutTrail(bool horizontal, bool hasOrigin)
{
	TrailLayouter::LayoutDirection direction;
	if (horizontal)
	{
		if (hasOrigin)
		{
			direction = TrailLayouter::LAYOUT_LEFT_RIGHT;
		}
		else
		{
			direction = TrailLayouter::LAYOUT_RIGHT_LEFT;
		}
	}
	else
	{
		if (hasOrigin)
		{
			direction = TrailLayouter::LAYOUT_TOP_BOTTOM;
		}
		else
		{
			direction = TrailLayouter::LAYOUT_BOTTOM_TOP;
		}
	}

	std::vector<std::shared_ptr<DummyNode>> visibleNodes;
	for (auto node: m_dummyNodes)
	{
		if (node->visible)
		{
			visibleNodes.push_back(node);
		}
	}

	TrailLayouter layout(direction);
	layout.layoutGraph(visibleNodes, m_dummyEdges, m_topLevelAncestorIds);
}

void GraphController::assignBundleIds()
{
	Id bundleId = 0;
	for (size_t i = m_dummyNodes.size(); i > 0; i--)
	{
		bundleId = m_dummyNodes[i - 1]->setBundleIdRecursive(bundleId);
	}
}

std::shared_ptr<DummyNode> GraphController::getDummyGraphNodeById(Id tokenId) const
{
	std::map<Id, std::shared_ptr<DummyNode>>::const_iterator it = m_dummyGraphNodes.find(tokenId);
	if (it != m_dummyGraphNodes.end())
	{
		return it->second;
	}

	for (const std::shared_ptr<DummyNode>& node: m_dummyNodes)
	{
		if (node->tokenId == tokenId)
		{
			return node;
		}
	}

	return nullptr;
}

DummyEdge* GraphController::getDummyGraphEdgeById(Id tokenId) const
{
	for (const std::shared_ptr<DummyEdge>& edge: m_dummyEdges)
	{
		if (edge->data && edge->data->getId() == tokenId)
		{
			return edge.get();
		}
	}

	return nullptr;
}

void GraphController::relayoutGraph(
	MessageBase* message,
	GraphView::GraphParams params,
	bool withCharacterIndex,
	const std::wstring& groupName)
{
	bool showsTrail = m_graph->getTrailMode() != Graph::TRAIL_NONE;

	setVisibility(setActive(utility::concat(m_activeNodeIds, m_activeEdgeIds), showsTrail));

	if (hasCharacterIndex() || withCharacterIndex)
	{
		addCharacterIndex();

		if (withCharacterIndex && m_dummyNodes.size())
		{
			// Use token Id of first node and make first 2 bits 1
			Id groupId = ~(~Id(0) >> 2) + m_dummyNodes[0]->tokenId;

			DummyNode* group = groupAllNodes(GroupType::DEFAULT, groupId);
			group->groupLayout = GroupLayout::LIST;
			group->interactive = false;
			group->name = groupName;
		}

		layoutNesting();
		layoutList();
	}
	else
	{
		if (!showsTrail)
		{
			groupNodesByParents(getView()->getGrouping());
		}

		layoutNesting();

		if (showsTrail)
		{
			layoutTrail(
				m_graph->getTrailMode() == Graph::TRAIL_HORIZONTAL, m_graph->hasTrailOrigin());
		}
		else
		{
			layoutGraph();
		}
	}

	buildGraph(message, params);
}

void GraphController::buildGraph(MessageBase* message, GraphView::GraphParams params)
{
	if (!message->isReplayed())
	{
		params.isIndexedList = params.scrollToTop;
		params.bezierEdges = m_useBezierEdges;
		params.disableInteraction = m_showsLegend;

		getView()->rebuildGraph(m_graph, m_dummyNodes, m_dummyEdges, params);
	}
}

void GraphController::forEachDummyNodeRecursive(std::function<void(DummyNode*)> func)
{
	for (const std::shared_ptr<DummyNode>& node: m_dummyNodes)
	{
		node->forEachDummyNodeRecursive(func);
	}
}

void GraphController::forEachDummyEdge(std::function<void(DummyEdge*)> func)
{
	for (const std::shared_ptr<DummyEdge>& edge: m_dummyEdges)
	{
		func(edge.get());
	}
}

void GraphController::createLegendGraph()
{
	Id id = ~Id(0) >> 1;
	std::map<Id, Vec2i> nodePositions;
	std::shared_ptr<Graph> graph = std::make_shared<Graph>();

	auto addText = [this](std::wstring text, int fontSizeDiff, Vec2i position) {
		std::shared_ptr<DummyNode> node = std::make_shared<DummyNode>(DummyNode::DUMMY_TEXT);
		node->name = text;
		node->visible = true;
		node->fontSizeDiff = fontSizeDiff;
		node->position = position;
		m_dummyNodes.push_back(node);
		return node;
	};

	auto addNode = [&id, &graph, &nodePositions](
					   NodeType::Type type,
					   const std::wstring& name,
					   Vec2i position,
					   DefinitionKind defKind = DEFINITION_EXPLICIT) {
		nodePositions.emplace(++id, position);
		return graph->createNode(id, type, NameHierarchy(name, NAME_DELIMITER_UNKNOWN), defKind);
	};

	auto addEdge = [&id, &graph](Edge::EdgeType type, Node* from, Node* to) {
		return graph->createEdge(++id, type, from, to);
	};

	auto addMember = [&id, &graph](Node* from, Node* to, AccessKind access = ACCESS_NONE) {
		if (access != ACCESS_NONE)
		{
			to->addComponent(std::make_shared<TokenComponentAccess>(access));
		}
		return graph->createEdge(++id, Edge::EDGE_MEMBER, from, to);
	};

	addText(L"Legend", 6, Vec2i(0, 0));

	size_t y = 50;
	size_t x = 0;

	// Layout
	{
		addText(L"Layout", 3, Vec2i(x, y));

		x = 50;
		y = 40;

		Node* base = addNode(NodeType::NODE_CLASS, L"Base Class", Vec2i(x + 220, y + 50));
		Node* main = addNode(NodeType::NODE_CLASS, L"Class", Vec2i(x + 200, y + 130));
		Node* derived = addNode(NodeType::NODE_CLASS, L"Derived Class", Vec2i(x + 210, y + 380));
		Node* user = addNode(NodeType::NODE_TYPE, L"Referencing Type", Vec2i(x - 10, y + 220));
		Node* usee = addNode(NodeType::NODE_TYPE, L"Referenced Type", Vec2i(x + 410, y + 220));

		addEdge(Edge::EDGE_INHERITANCE, main, base);
		addEdge(Edge::EDGE_INHERITANCE, derived, main);

		{
			Edge* edge = addEdge(Edge::EDGE_AGGREGATION, user, main);
			std::shared_ptr<TokenComponentAggregation> aggregationComp =
				std::make_shared<TokenComponentAggregation>();
			for (size_t i = 0; i < 10; i++)
			{
				aggregationComp->addAggregationId(++id, true);
			}
			edge->addComponent(aggregationComp);
		}

		{
			Edge* edge = addEdge(Edge::EDGE_AGGREGATION, main, usee);
			std::shared_ptr<TokenComponentAggregation> aggregationComp =
				std::make_shared<TokenComponentAggregation>();
			for (size_t i = 0; i < 10; i++)
			{
				aggregationComp->addAggregationId(++id, true);
			}
			edge->addComponent(aggregationComp);
		}

		Node* publicMethod = addNode(NodeType::NODE_METHOD, L"public method", Vec2i());
		Node* privateField = addNode(NodeType::NODE_FIELD, L"private field", Vec2i());

		addMember(main, publicMethod, ACCESS_PUBLIC);
		addMember(main, privateField, ACCESS_PRIVATE);

		y += 480;
		x += 10;

		Node* func = addNode(NodeType::NODE_FUNCTION, L"function", Vec2i(x + 220, y));
		Node* caller = addNode(NodeType::NODE_FUNCTION, L"calling function", Vec2i(x, y));
		Node* var = addNode(
			NodeType::NODE_GLOBAL_VARIABLE, L"accessed variable", Vec2i(x + 410, y - 50));
		Node* called = addNode(NodeType::NODE_FUNCTION, L"called function", Vec2i(x + 410, y - 10));
		Node* type = addNode(NodeType::NODE_TYPE, L"Referenced Type", Vec2i(x + 410, y + 30));

		addEdge(Edge::EDGE_CALL, func, called);
		addEdge(Edge::EDGE_CALL, caller, func);
		addEdge(Edge::EDGE_USAGE, func, var);
		addEdge(Edge::EDGE_TYPE_USAGE, func, type);
	}

	x = 0;
	y = 610;
	size_t dx = 200;
	size_t dy = 50;

	// Nodes
	{
		size_t i = 0;
		addText(L"Nodes", 3, Vec2i(x, y));

		addNode(NodeType::NODE_FILE, L"File", Vec2i(x, y + dy * ++i));
		addNode(NodeType::NODE_FILE, L"Non-Indexed File", Vec2i(x, y + dy * ++i), DEFINITION_NONE);
		Node* incompleteFile = addNode(
			NodeType::NODE_FILE, L"Incomplete File", Vec2i(x, y + dy * ++i));
		incompleteFile->addComponent(std::make_shared<TokenComponentFilePath>(FilePath(), false));

		addNode(NodeType::NODE_MACRO, L"Macro", Vec2i(x, y + dy * ++i));
		addNode(NodeType::NODE_ANNOTATION, L"Annotation", Vec2i(x, y + dy * ++i));

		addNode(NodeType::NODE_MODULE, L"module", Vec2i(x, y + dy * ++i));
		y -= 15;
		addNode(NodeType::NODE_NAMESPACE, L"namespace", Vec2i(x, y + dy * ++i));
		y -= 15;
		addNode(NodeType::NODE_PACKAGE, L"package", Vec2i(x, y + dy * ++i));
		y -= 15;

		addNode(NodeType::NODE_TYPE, L"Type", Vec2i(x, y + dy * ++i));
		addNode(NodeType::NODE_TYPE, L"Non-indexed Type", Vec2i(x, y + dy * ++i), DEFINITION_NONE);

		addNode(NodeType::NODE_GLOBAL_VARIABLE, L"variable", Vec2i(x, y + dy * ++i));
		y -= 15;
		addNode(
			NodeType::NODE_GLOBAL_VARIABLE,
			L"non-indexed variable",
			Vec2i(x, y + dy * ++i),
			DEFINITION_NONE);
		y -= 15;

		addNode(NodeType::NODE_FUNCTION, L"function", Vec2i(x, y + dy * ++i));
		y -= 15;
		addNode(
			NodeType::NODE_FUNCTION, L"non-indexed function", Vec2i(x, y + dy * ++i), DEFINITION_NONE);
		y -= 15;

		Node* typeNode = addNode(NodeType::NODE_TYPE, L"Type with Members", Vec2i(x, y + dy * ++i));
		Node* publicMethod = addNode(NodeType::NODE_METHOD, L"public method", Vec2i());
		Node* protectedMethod = addNode(NodeType::NODE_METHOD, L"protected method", Vec2i());
		Node* privateMethod = addNode(NodeType::NODE_METHOD, L"private method", Vec2i());
		Node* defaultMethod = addNode(NodeType::NODE_METHOD, L"default method", Vec2i());
		Node* publicField = addNode(NodeType::NODE_FIELD, L"public field", Vec2i());
		Node* protectedField = addNode(NodeType::NODE_FIELD, L"protected field", Vec2i());
		Node* privateField = addNode(NodeType::NODE_FIELD, L"private field", Vec2i());
		Node* defaultField = addNode(NodeType::NODE_FIELD, L"default field", Vec2i());

		addMember(typeNode, publicMethod, ACCESS_PUBLIC);
		addMember(typeNode, publicField, ACCESS_PUBLIC);
		addMember(typeNode, protectedMethod, ACCESS_PROTECTED);
		addMember(typeNode, protectedField, ACCESS_PROTECTED);
		addMember(typeNode, privateMethod, ACCESS_PRIVATE);
		addMember(typeNode, privateField, ACCESS_PRIVATE);
		addMember(typeNode, defaultMethod, ACCESS_DEFAULT);
		addMember(typeNode, defaultField, ACCESS_DEFAULT);

		y -= 15;
		i += 9;

		addNode(NodeType::NODE_CLASS, L"Class", Vec2i(x, y + dy * ++i));
		addNode(NodeType::NODE_INTERFACE, L"Interface", Vec2i(x, y + dy * ++i));

		addNode(NodeType::NODE_STRUCT, L"Struct", Vec2i(x, y + dy * ++i));
		addNode(NodeType::NODE_UNION, L"Union", Vec2i(x, y + dy * ++i));

		addNode(NodeType::NODE_TYPEDEF, L"TypeDef", Vec2i(x, y + dy * ++i));
		Node* enumNode = addNode(NodeType::NODE_ENUM, L"Enum", Vec2i(x, y + dy * ++i));
		Node* enumConstantNode = addNode(NodeType::NODE_ENUM_CONSTANT, L"ENUM_CONSTANT", Vec2i());
		addMember(enumNode, enumConstantNode);
		y += 10;
		i += 1;

		Node* genericNode = addNode(
			NodeType::NODE_TYPE, L"JavaGenericType<ParameterType>", Vec2i(x, y + dy * ++i));
		Node* genericParameterNode = addNode(
			NodeType::NODE_TYPE_PARAMETER, L"ParameterType", Vec2i());
		addMember(genericNode, genericParameterNode, ACCESS_TYPE_PARAMETER);
		i += 2;

		y += 10;

		std::shared_ptr<DummyNode> groupNode = std::make_shared<DummyNode>(DummyNode::DUMMY_GROUP);
		groupNode->name = L"Group Node";
		groupNode->visible = true;
		groupNode->groupType = GroupType::DEFAULT;
		groupNode->position = Vec2i(x, y + dy * ++i);
		m_dummyNodes.push_back(groupNode);
		y += 25;

		std::shared_ptr<DummyNode> bundleNode = std::make_shared<DummyNode>(DummyNode::DUMMY_BUNDLE);
		bundleNode->name = L"Bundle Node";
		bundleNode->visible = true;
		bundleNode->position = Vec2i(x, y + dy * ++i);
		m_dummyNodes.push_back(bundleNode);
	}

	y = 610;
	x = 380;

	// Edges
	{
		addText(L"Edges", 3, Vec2i(x, y));
		size_t i = 0;

		{
			addText(L"file include", 0, Vec2i(x, y + dy * ++i));
			Node* file = addNode(NodeType::NODE_FILE, L"File", Vec2i(x, y + dy * ++i));
			Node* fileB = addNode(NodeType::NODE_FILE, L"File", Vec2i(x + dx, y + dy * i));
			addEdge(Edge::EDGE_INCLUDE, file, fileB);
		}

		{
			addText(L"class import", 0, Vec2i(x, y + dy * ++i));
			Node* file = addNode(NodeType::NODE_FILE, L"File", Vec2i(x, y + dy * ++i));
			Node* type = addNode(NodeType::NODE_TYPE, L"Class", Vec2i(x + dx, y + dy * i));
			addEdge(Edge::EDGE_IMPORT, file, type);
		}

		{
			addText(L"macro use", 0, Vec2i(x, y + dy * ++i));
			Node* file = addNode(NodeType::NODE_FILE, L"File", Vec2i(x, y + dy * ++i));
			Node* macro = addNode(NodeType::NODE_MACRO, L"Macro", Vec2i(x + dx, y + dy * i));
			addEdge(Edge::EDGE_MACRO_USAGE, file, macro);
		}

		{
			addText(L"annotation use", 0, Vec2i(x, y + dy * ++i));
			Node* type = addNode(NodeType::NODE_TYPE, L"Type", Vec2i(x, y + dy * ++i));
			Node* macro = addNode(
				NodeType::NODE_ANNOTATION, L"Annotation", Vec2i(x + dx, y + dy * i));
			addEdge(Edge::EDGE_ANNOTATION_USAGE, type, macro);
		}

		{
			addText(L"aggregation", 0, Vec2i(x, y + dy * ++i));
			Node* typeA = addNode(NodeType::NODE_TYPE, L"Type A", Vec2i(x, y + dy * ++i));
			Node* typeB = addNode(NodeType::NODE_TYPE, L"Type B", Vec2i(x + dx, y + dy * i));
			Edge* edge = addEdge(Edge::EDGE_AGGREGATION, typeA, typeB);
			std::shared_ptr<TokenComponentAggregation> aggregationComp =
				std::make_shared<TokenComponentAggregation>();
			for (size_t i = 0; i < 10; i++)
			{
				aggregationComp->addAggregationId(++id, true);
			}
			edge->addComponent(aggregationComp);
		}

		{
			addText(L"type use", 0, Vec2i(x, y + dy * ++i));
			Node* function = addNode(NodeType::NODE_FUNCTION, L"function", Vec2i(x, y + dy * ++i));
			Node* type = addNode(NodeType::NODE_TYPE, L"Type", Vec2i(x + dx, y + dy * i));
			addEdge(Edge::EDGE_TYPE_USAGE, function, type);
		}

		{
			addText(L"function call", 0, Vec2i(x, y + dy * ++i));
			Node* function = addNode(NodeType::NODE_FUNCTION, L"function", Vec2i(x, y + dy * ++i));
			Node* functionB = addNode(
				NodeType::NODE_FUNCTION, L"function", Vec2i(x + dx, y + dy * i));
			addEdge(Edge::EDGE_CALL, function, functionB);
		}

		{
			addText(L"variable access", 0, Vec2i(x, y + dy * ++i));
			Node* function = addNode(NodeType::NODE_FUNCTION, L"function", Vec2i(x, y + dy * ++i));
			Node* variable = addNode(
				NodeType::NODE_GLOBAL_VARIABLE, L"variable", Vec2i(x + dx, y + dy * i));
			addEdge(Edge::EDGE_USAGE, function, variable);
		}

		{
			addText(L"class inheritance", 0, Vec2i(x, y + dy * ++i));
			Node* base = addNode(NodeType::NODE_CLASS, L"Base Class", Vec2i(x, y + dy * (i + 1)));
			Node* derived = addNode(
				NodeType::NODE_CLASS, L"Derived Class", Vec2i(x, y + dy * (i + 3)));
			addEdge(Edge::EDGE_INHERITANCE, derived, base);

			Node* base2 = addNode(
				NodeType::NODE_CLASS, L"Base Class", Vec2i(x + 180, y + dy * (i + 1)));
			Node* derived2 = addNode(
				NodeType::NODE_CLASS, L"Derived Derived Class", Vec2i(x + 180, y + dy * (i + 3)));
			Edge* edge = addEdge(Edge::EDGE_INHERITANCE, derived2, base2);
			edge->addComponent(
				std::make_shared<TokenComponentInheritanceChain>(std::vector<Id>({1, 2})));
			i += 3;
		}

		{
			addText(L"method override", 0, Vec2i(x, y + dy * ++i));
			Node* base = addNode(NodeType::NODE_CLASS, L"Base Class", Vec2i(x, y + dy * ++i));
			i += 3;
			Node* derived = addNode(NodeType::NODE_CLASS, L"Derived Class", Vec2i(x, y + dy * i));
			Node* baseMethod = addNode(NodeType::NODE_METHOD, L"method", Vec2i());
			Node* derivedMethod = addNode(NodeType::NODE_METHOD, L"method", Vec2i());
			addMember(base, baseMethod, ACCESS_PUBLIC);
			addMember(derived, derivedMethod, ACCESS_PUBLIC);
			addEdge(Edge::EDGE_OVERRIDE, derivedMethod, baseMethod);
			i += 2;
		}

		{
			addText(L"template specialization", 0, Vec2i(x, y + dy * ++i));
			Node* templateFunctionNode = addNode(
				NodeType::NODE_FUNCTION,
				L"template_function<typename ParameterType>",
				Vec2i(x, y + dy * ++i));
			y += 20;
			Node* templateFunctionSpecializationNode = addNode(
				NodeType::NODE_FUNCTION,
				L"template_function<ArgumentType>",
				Vec2i(x, y + dy * ++i),
				DEFINITION_IMPLICIT);
			addEdge(
				Edge::EDGE_TEMPLATE_SPECIALIZATION,
				templateFunctionSpecializationNode,
				templateFunctionNode);

			Node* templateNode = addNode(
				NodeType::NODE_TYPE, L"TemplateType<typename ParameterType>", Vec2i(x, y + dy * ++i));
			y += 30;
			Node* templateSpecializationNode = addNode(
				NodeType::NODE_TYPE,
				L"TemplateType<ArgumentType>",
				Vec2i(x, y + dy * ++i),
				DEFINITION_IMPLICIT);
			Node* argumentNode = addNode(
				NodeType::NODE_TYPE, L"ArgumentType", Vec2i(x + 270, y + dy * i));
			addEdge(Edge::EDGE_TEMPLATE_SPECIALIZATION, templateSpecializationNode, templateNode);
			addEdge(Edge::EDGE_TYPE_USAGE, templateSpecializationNode, argumentNode);
		}

		{
			addText(L"template member specialization", 0, Vec2i(x, y + dy * ++i));
			Node* templateNode = addNode(
				NodeType::NODE_TYPE, L"TemplateType<typename ParameterType>", Vec2i(x, y + dy * ++i));
			Node* templateMethodNode = addNode(NodeType::NODE_METHOD, L"method", Vec2i());
			addMember(templateNode, templateMethodNode);

			i += 1;

			Node* templateSpecializationNode = addNode(
				NodeType::NODE_TYPE,
				L"TemplateType<ArgumentType>",
				Vec2i(x, y + dy * ++i + 20),
				DEFINITION_IMPLICIT);
			Node* templateSpecializationMethodNode = addNode(
				NodeType::NODE_METHOD, L"method", Vec2i(), DEFINITION_IMPLICIT);
			addMember(templateSpecializationNode, templateSpecializationMethodNode);
			addEdge(
				Edge::EDGE_TEMPLATE_SPECIALIZATION,
				templateSpecializationMethodNode,
				templateMethodNode);
		}
	}

	std::vector<std::shared_ptr<DummyNode>> nodes = m_dummyNodes;
	createDummyGraphAndSetActiveAndVisibility({}, graph, false);
	m_dummyNodes = utility::concat(nodes, m_dummyNodes);

	for (std::shared_ptr<DummyNode> node: m_dummyNodes)
	{
		if (node->tokenId)
		{
			auto it = nodePositions.find(node->tokenId);
			if (it != nodePositions.end())
			{
				node->position = it->second;
			}
		}
	}
}
