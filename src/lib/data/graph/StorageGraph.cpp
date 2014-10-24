#include "data/graph/StorageGraph.h"

#include "data/graph/token_component/TokenComponentName.h"
#include "utility/logging/logging.h"
#include "utility/utilityString.h"

StorageGraph::StorageGraph()
{
}

StorageGraph::~StorageGraph()
{
}

Node* StorageGraph::createNodeHierarchy(Node::NodeType type, SearchIndex::SearchNode* searchNode)
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
	Node::NodeType type, SearchIndex::SearchNode* searchNode, std::shared_ptr<TokenComponentSignature> signature
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
		[to](Edge* e)
		{
			return e->getTo() == to;
		}
	);

	if (edge)
	{
		return edge;
	}

	return insertEdge(type, from, to);
}

Node* StorageGraph::insertNodeHierarchy(Node::NodeType type, SearchIndex::SearchNode* searchNode)
{
	std::deque<SearchIndex::SearchNode*> searchNodes = searchNode->getParentsWithoutTokenId();

	if (!searchNodes.size())
	{
		LOG_ERROR("There are no nodes without a set tokenId so this method shouldn't have been called.");
		return nullptr;
	}

	Node* parentNode = nullptr;
	SearchIndex::SearchNode* parentSearchNode = searchNodes.front()->getParent();
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

Node* StorageGraph::insertNode(Node::NodeType type, Node* parentNode, SearchIndex::SearchNode* searchNode)
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
