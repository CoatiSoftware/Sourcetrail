#include "data/graph/StorageGraph.h"

#include "data/graph/token_component/TokenComponentAggregation.h"
#include "data/graph/token_component/TokenComponentName.h"
#include "utility/logging/logging.h"
#include "utility/utilityString.h"

StorageGraph::StorageGraph()
{
}

StorageGraph::~StorageGraph()
{
}

Node* StorageGraph::createNodeHierarchy(Node::NodeType type, SearchNode* searchNode)
{
	Node* node = getNodeById(searchNode->getFirstTokenId());
	if (!node)
	{
		return insertNodeHierarchy(type, searchNode);
	}

	if (node->getType() < type)
	{
		node->setType(type);
	}

	return node;
}

Node* StorageGraph::createNodeHierarchyWithDistinctSignature(
	Node::NodeType type, SearchNode* searchNode, std::shared_ptr<TokenComponentSignature> signature
){
	Node* node = getNodeById(searchNode->getFirstTokenId());
	if (!node)
	{
		node = insertNodeHierarchy(type, searchNode);
	}
	else
	{
		std::function<bool(Node*)> findSignature =
			[signature](Node* n)
			{
				TokenComponentSignature* sig = n->getComponent<TokenComponentSignature>();
				return sig && *sig == *signature.get();
			};

		Node* parentNode = node->getParentNode();
		if (parentNode)
		{
			node = parentNode->findChildNode(findSignature);
		}
		else
		{
			node = findNode(findSignature);
		}

		if (!node)
		{
			node = insertNode(type, parentNode, searchNode);
		}
		else
		{
			if (node->getType() < type)
			{
				node->setType(type);
			}
			return node;
		}
	}

	node->addComponentSignature(signature);
	return node;
}

Edge* StorageGraph::createEdge(Edge::EdgeType type, Node* from, Node* to)
{
	Edge* edge = from->findEdgeOfType(type,
		[from, to](Edge* e)
		{
			return e->getFrom() == from && e->getTo() == to;
		}
	);

	if (edge)
	{
		return edge;
	}

	edge = insertEdge(type, from, to);

	if (from->getLastParentNode() != to->getLastParentNode())
	{
		Id edgeId = edge->getId();
		updateAggregationEdges(from->getParentNode(), to, edgeId, 0);
		updateAggregationEdges(from, to->getParentNode(), edgeId, 0);
	}

	return edge;
}

void StorageGraph::removeEdge(Edge* edge)
{
	Node* from = edge->getFrom();
	Node* to = edge->getTo();

	if (from->getLastParentNode() != to->getLastParentNode())
	{
		Id edgeId = edge->getId();
		updateAggregationEdges(from->getParentNode(), to, 0, edgeId);
		updateAggregationEdges(from, to->getParentNode(), 0, edgeId);
	}

	Graph::removeEdge(edge);
}

Node* StorageGraph::insertNodeHierarchy(Node::NodeType type, SearchNode* searchNode)
{
	std::deque<SearchNode*> searchNodes = searchNode->getParentsWithoutTokenId();

	if (!searchNodes.size())
	{
		LOG_ERROR("There are no nodes without a set tokenId so this method shouldn't have been called.");
		return nullptr;
	}

	Node* parentNode = nullptr;
	SearchNode* parentSearchNode = searchNodes.front()->getParent();
	if (parentSearchNode)
	{
		parentNode = getNodeById(parentSearchNode->getFirstTokenId());
	}

	while (searchNodes.size() > 0)
	{
		searchNode = searchNodes.front();
		searchNodes.pop_front();

		parentNode = insertNode(searchNodes.size() > 0 ? Node::NODE_UNDEFINED : type, parentNode, searchNode);
	}

	return parentNode;
}

Node* StorageGraph::insertNode(Node::NodeType type, Node* parentNode, SearchNode* searchNode)
{
	std::shared_ptr<Node> node =
		std::make_shared<Node>(type, std::make_shared<TokenComponentNameReferenced>(searchNode));
	m_nodes.emplace(node->getId(), node);

	searchNode->addTokenId(node->getId());

	if (parentNode)
	{
		createEdge(Edge::EDGE_MEMBER, parentNode, node.get());
	}

	return node.get();
}

Edge* StorageGraph::insertEdge(Edge::EdgeType type, Node* from, Node* to)
{
	std::shared_ptr<Edge> edgePtr = std::make_shared<Edge>(type, from, to);
	m_edges.emplace(edgePtr->getId(), edgePtr);
	return edgePtr.get();
}

void StorageGraph::updateAggregationEdges(Node* from, Node* to, Id addEdgeId, Id removeEdgeId)
{
	if (!from || !to || from == to)
	{
		return;
	}

	const Node::NodeTypeMask mask = Node::NODE_UNDEFINED_TYPE | Node::NODE_CLASS | Node::NODE_STRUCT | Node::NODE_ENUM;
	const Node::NodeTypeMask varFuncMask =
		Node::NODE_UNDEFINED_FUNCTION | Node::NODE_FUNCTION | Node::NODE_UNDEFINED_VARIABLE | Node::NODE_GLOBAL_VARIABLE;

	if ((from->isType(mask) && to->isType(mask | varFuncMask)) ||
		(from->isType(mask | varFuncMask) && to->isType(mask)))
	{
		Edge* edge = from->findEdgeOfType(Edge::EDGE_AGGREGATION,
			[from, to](Edge* e)
			{
				const Node* f = e->getFrom();
				const Node* t = e->getTo();
				return (f == from && t == to) || (f == to && t == from);
			}
		);

		if (!edge && addEdgeId)
		{
			edge = insertEdge(Edge::EDGE_AGGREGATION, from, to);
			edge->addComponentAggregation(std::make_shared<TokenComponentAggregation>());
		}

		if (addEdgeId)
		{
			bool forward = (edge->getFrom() == from);
			edge->getComponent<TokenComponentAggregation>()->addAggregationId(addEdgeId, forward);
		}

		if (edge && removeEdgeId)
		{
			edge->getComponent<TokenComponentAggregation>()->removeAggregationId(removeEdgeId);
		}

		if (edge && edge->getComponent<TokenComponentAggregation>()->getAggregationCount() == 0)
		{
			Graph::removeEdge(edge);
		}
	}

	updateAggregationEdges(from->getParentNode(), to, addEdgeId, removeEdgeId);
	updateAggregationEdges(from, to->getParentNode(), addEdgeId, removeEdgeId);
}
