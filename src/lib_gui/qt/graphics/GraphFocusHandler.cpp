#include "GraphFocusHandler.h"

#include "MessageFocusChanged.h"
#include "QtGraphEdge.h"
#include "QtGraphNode.h"
#include "utility.h"

GraphFocusHandler::GraphFocusHandler(GraphFocusClient* client): m_client(client) {}

void GraphFocusHandler::clear()
{
	m_focusNode = nullptr;
	m_focusEdge = nullptr;
	m_lastFocusId = 0;
}

void GraphFocusHandler::focus()
{
	if (m_focusNode)
	{
		m_focusNode->setIsFocused(true);
	}
	else if (m_focusEdge)
	{
		m_focusEdge->setIsFocused(true);
	}
	else
	{
		focusInitialNode();
	}
}

void GraphFocusHandler::defocus()
{
	if (m_focusNode)
	{
		m_focusNode->setIsFocused(false);
	}
	else if (m_focusEdge)
	{
		m_focusEdge->setIsFocused(false);
	}
}

void GraphFocusHandler::focusInitialNode()
{
	if (m_focusNode != nullptr || m_focusEdge != nullptr)
	{
		return;
	}

	QtGraphNode* nodeToFocus = nullptr;
	if (m_lastFocusId)
	{
		nodeToFocus = QtGraphNode::findNodeRecursive(m_client->getGraphNodes(), m_lastFocusId);
	}

	if (!nodeToFocus)
	{
		nodeToFocus = m_client->getActiveNode();
	}

	if (!nodeToFocus)
	{
		nodeToFocus = findChildNodeRecursive(m_client->getGraphNodes(), true);
	}

	if (nodeToFocus)
	{
		focusNode(nodeToFocus);
	}
}

void GraphFocusHandler::focusTokenId(
	const std::list<QtGraphNode*>& nodes, const std::list<QtGraphEdge*>& edges, Id tokenId)
{
	m_focusNode = nullptr;
	m_focusEdge = nullptr;

	QtGraphNode* nodeToFocus = QtGraphNode::findNodeRecursive(nodes, tokenId);
	if (nodeToFocus)
	{
		focusNode(nodeToFocus);
		return;
	}

	QtGraphEdge* edgeToFocus = nullptr;
	for (QtGraphEdge* edge: edges)
	{
		if (edge->getTokenId() == tokenId)
		{
			edgeToFocus = edge;
			break;
		}
	}

	if (edgeToFocus)
	{
		focusEdge(edgeToFocus);
	}
}

void GraphFocusHandler::refocusNode(
	const std::list<QtGraphNode*>& newNodes, Id oldActiveTokenId, Id newActiveTokenId)
{
	m_focusNode = nullptr;
	m_focusEdge = nullptr;

	if (m_lastFocusId && (m_lastFocusId == newActiveTokenId || oldActiveTokenId == newActiveTokenId))
	{
		QtGraphNode* nodeToFocus = QtGraphNode::findNodeRecursive(newNodes, m_lastFocusId);
		if (nodeToFocus)
		{
			m_focusNode = nodeToFocus;
			nodeToFocus->setIsFocused(true);
		}
	}
	else
	{
		m_lastFocusId = 0;
	}
}

void GraphFocusHandler::focusNext(Direction direction, bool navigateEdges)
{
	QtGraphEdge* nextEdge = nullptr;

	if (m_focusNode)
	{
		if (navigateEdges)
		{
			nextEdge = findNextEdge(m_focusNode->sceneBoundingRect().center(), direction);
			if (nextEdge)
			{
				focusEdge(nextEdge);
			}
		}

		if (!nextEdge)
		{
			QtGraphNode* nextNode = findNextNode(m_focusNode, direction);
			if (nextNode)
			{
				focusNode(nextNode);
			}
		}
	}
	else if (m_focusEdge)
	{
		if (navigateEdges)
		{
			nextEdge = findNextEdge(m_focusEdge->getBoundingRect().center(), direction, m_focusEdge);
			if (nextEdge)
			{
				focusEdge(nextEdge);
			}
		}

		if (!nextEdge)
		{
			QtGraphNode* nextNode = findNextNode(m_focusEdge, direction);
			if (nextNode)
			{
				focusNode(nextNode);
			}
		}
	}

	if (m_focusNode)
	{
		m_client->ensureNodeVisible(m_focusNode);
	}
}

void GraphFocusHandler::focusNode(QtGraphNode* node)
{
	m_client->focusView(true);

	if (node == m_focusNode)
	{
		return;
	}

	defocusGraph();

	if (node->isFocusable())
	{
		node->setIsFocused(true);
		m_focusNode = node;
		m_lastFocusId = node->getTokenId();

		MessageFocusChanged(MessageFocusChanged::ViewType::GRAPH, node->getTokenId()).dispatch();
	}
}

void GraphFocusHandler::defocusNode(QtGraphNode* node)
{
	QtGraphNode* parent = node->getParent();
	while (parent && !parent->isFocusable())
	{
		parent = parent->getParent();
	}

	if (parent)
	{
		focusNode(parent);
	}
}

void GraphFocusHandler::focusEdge(QtGraphEdge* edge)
{
	m_client->focusView(true);

	if (edge == m_focusEdge)
	{
		return;
	}

	defocusGraph();

	if (edge->isFocusable())
	{
		edge->setIsFocused(true);
		m_focusEdge = edge;
		m_lastFocusId = 0;

		MessageFocusChanged(MessageFocusChanged::ViewType::GRAPH, edge->getTokenId()).dispatch();
	}
}

void GraphFocusHandler::defocusEdge(QtGraphEdge* edge) {}

void GraphFocusHandler::defocusGraph()
{
	if (m_focusNode)
	{
		m_focusNode->setIsFocused(false);
		m_focusNode = nullptr;
	}

	if (m_focusEdge)
	{
		m_focusEdge->setIsFocused(false);
		m_focusEdge = nullptr;
	}
}

void GraphFocusHandler::activateFocus(bool openInTab)
{
	if (m_focusNode)
	{
		if (openInTab)
		{
			m_focusNode->onMiddleClick();
		}
		else
		{
			m_focusNode->onClick();
		}
	}
	else if (m_focusEdge)
	{
		m_focusEdge->onClick();
	}
}

void GraphFocusHandler::expandFocus()
{
	if (m_focusNode)
	{
		m_lastFocusId = m_focusNode->onCollapseExpand();
	}
	else if (m_focusEdge)
	{
		m_focusEdge->onClick();
	}
}

QtGraphNode* GraphFocusHandler::findNextNode(QtGraphNode* node, Direction direction)
{
	switch (direction)
	{
	case Direction::UP:
	{
		QtGraphNode* nextNode = findSibling(node, direction);
		if (nextNode)
		{
			QtGraphNode* childNode = nextNode;
			QtGraphNode* lastChildNode = nullptr;
			while (childNode)
			{
				childNode = findChildNodeRecursive(childNode->getSubNodes(), false);
				if (childNode)
				{
					lastChildNode = childNode;
				}
			}

			if (lastChildNode && lastChildNode->getPosition().y() < node->getPosition().y())
			{
				return lastChildNode;
			}

			return nextNode;
		}

		QtGraphNode* parent = node->getParent();
		while (parent && !parent->isFocusable())
		{
			parent = parent->getParent();
		}

		if (parent)
		{
			return parent;
		}
		break;
	}

	case Direction::DOWN:
	{
		if (node->getSubNodes().size())
		{
			QtGraphNode* nextNode = findChildNodeRecursive(node->getSubNodes(), true);
			if (nextNode)
			{
				return nextNode;
			}
		}
	}

	case Direction::LEFT:
	case Direction::RIGHT:
	{
		QtGraphNode* nextNode = findSibling(node, direction);
		if (nextNode)
		{
			return nextNode;
		}
		break;
	}
	}

	return nullptr;
}

QtGraphNode* GraphFocusHandler::findNextNode(QtGraphEdge* edge, Direction direction)
{
	if (edge->isHorizontal())
	{
		if ((direction == Direction::LEFT) ==
			(edge->getOwner()->getPosition().x() < edge->getTarget()->getPosition().x()))
		{
			return edge->getOwner();
		}
		else
		{
			return edge->getTarget();
		}
	}
	else
	{
		if ((direction == Direction::UP) ==
			(edge->getOwner()->getPosition().y() < edge->getTarget()->getPosition().y()))
		{
			return edge->getOwner();
		}
		else
		{
			return edge->getTarget();
		}
	}

	return nullptr;
}

QtGraphEdge* GraphFocusHandler::findNextEdge(QPointF pos, Direction direction, QtGraphEdge* previousEdge)
{
	QtGraphEdge* closestEdge = nullptr;
	QRectF previousEdgeRect;
	if (previousEdge)
	{
		previousEdgeRect = previousEdge->getBoundingRect();
	}

	qreal minDist = 0;

	for (QtGraphEdge* edge: m_client->getGraphEdges())
	{
		if (edge == previousEdge || !edge->isFocusable())
		{
			continue;
		}

		QRectF edgeRect = edge->getBoundingRect();
		QPointF edgePos = edgeRect.center();

		bool top = edgePos.y() < pos.y();
		bool bottom = edgePos.y() > pos.y();
		bool left = edgePos.x() < pos.x();
		bool right = edgePos.x() > pos.x();

		bool isDir = (direction == Direction::UP && top) ||
			(direction == Direction::DOWN && bottom) || (direction == Direction::LEFT && left) ||
			(direction == Direction::RIGHT && right);

		if (isDir)
		{
			qreal distX = std::abs(pos.x() - edgePos.x());
			qreal distY = std::abs(pos.y() - edgePos.y());

			const bool vertical = (direction == Direction::UP || direction == Direction::DOWN);

			qreal distXMult = vertical ? 2.0 : 1.0;
			qreal distYMult = vertical ? 1.0 : 2.0;

			// only use one dimension for distance calculation in aggregation graphs to not miss
			// certain edges
			if (edge->isBezierEdge() && m_client->getGraphNodes().size() == 2)
			{
				distXMult = vertical ? 0.0 : 1.0;
				distYMult = vertical ? 1.0 : 0.0;
			}

			qreal dist = distX * distXMult + distY * distYMult;
			if (!minDist || dist < minDist)
			{
				minDist = dist;
				closestEdge = edge;
			}
		}
	}

	return closestEdge;
}

QtGraphNode* GraphFocusHandler::findChildNodeRecursive(const std::list<QtGraphNode*>& nodes, bool first)
{
	QtGraphNode* result = nullptr;

	for (QtGraphNode* node: nodes)
	{
		if (node->isFocusable())
		{
			result = node;
			if (first)
			{
				break;
			}
		}

		QtGraphNode* newResult = findChildNodeRecursive(node->getSubNodes(), first);
		if (newResult)
		{
			result = newResult;
			if (first)
			{
				break;
			}
		}
	}

	return result;
}

QtGraphNode* GraphFocusHandler::findSibling(const QtGraphNode* node, Direction direction)
{
	QtGraphNode* nextSibling = nullptr;
	Vec2i pos = node->getPosition();
	Vec4i rect = node->getBoundingRect();
	int minDist = 0;

	for (auto siblings: getSiblingsHierarchyRecursive(node))
	{
		for (QtGraphNode* sibling: siblings)
		{
			if (sibling == node || !sibling->isFocusable())
			{
				continue;
			}

			Vec4i rectS = sibling->getBoundingRect();

			const bool top = rectS.w() < rect.y();
			const bool bottom = rectS.y() > rect.w();
			const bool left = rectS.z() < rect.x();
			const bool right = rectS.x() > rect.z();

			const bool isDir = (direction == Direction::UP &&
								(top || (!top && !bottom && !left && !right))) ||
				(direction == Direction::DOWN && bottom) ||
				(direction == Direction::LEFT && left) || (direction == Direction::RIGHT && right);

			if (isDir)
			{
				int distX = 0;
				int distY = 0;

				int distXMult = (direction == Direction::UP || direction == Direction::DOWN) ? 2 : 1;
				int distYMult = (direction == Direction::LEFT || direction == Direction::RIGHT) ? 2
																								: 1;
				if (top)
					distY = rect.y() - rectS.w();
				if (bottom)
					distY = rectS.y() - rect.w();
				if (left)
					distX = rect.x() - rectS.z();
				if (right)
					distX = rectS.x() - rect.z();

				int dist = distX * distXMult + distY * distYMult;
				if (!nextSibling || dist < minDist)
				{
					nextSibling = sibling;
					minDist = dist;
				}
			}
		}

		if (nextSibling)
		{
			break;
		}
	}

	return nextSibling;
}

std::vector<std::vector<QtGraphNode*>> GraphFocusHandler::getSiblingsHierarchyRecursive(
	const QtGraphNode* node)
{
	std::vector<std::vector<QtGraphNode*>> siblingsList;

	QtGraphNode* parent = node->getParent();
	while (parent && !parent->isDataNode() && !parent->isGroupNode())
	{
		parent = parent->getParent();
	}

	if (parent)
	{
		std::vector<QtGraphNode*> siblings;
		addSiblingsRecursive(parent->getSubNodes(), siblings);
		if (siblings.size())
		{
			siblingsList.push_back(siblings);
		}

		utility::append(siblingsList, getSiblingsHierarchyRecursive(parent));
	}
	else
	{
		siblingsList.push_back(utility::toVector(m_client->getGraphNodes()));
	}

	return siblingsList;
}

void GraphFocusHandler::addSiblingsRecursive(
	const std::list<QtGraphNode*>& nodes, std::vector<QtGraphNode*>& siblings)
{
	for (QtGraphNode* node: nodes)
	{
		if (node->isFocusable())
		{
			siblings.push_back(node);
		}
		else
		{
			addSiblingsRecursive(node->getSubNodes(), siblings);
		}
	}
}
