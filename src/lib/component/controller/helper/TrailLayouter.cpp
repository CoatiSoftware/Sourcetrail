#include "component/controller/helper/TrailLayouter.h"

#include <iostream>

TrailLayouter::TrailLayouter(LayoutDirection dir)
	: m_direction(dir)
{
}

void TrailLayouter::layoutGraph(
	std::vector<std::shared_ptr<DummyNode>>& dummyNodes,
	const std::vector<std::shared_ptr<DummyEdge>>& dummyEdges,
	const std::map<Id, Id>& topLevelAncestorIds)
{
	buildGraph(dummyNodes, dummyEdges, topLevelAncestorIds);

	makeAcyclic();

	assignLongestPathLevels();
	assignRemainingLevels();

	addVirtualNodes();

	buildColumns();
	reduceEdgeCrossings();
	layout();

	retrievePositions(topLevelAncestorIds);

	// print();
}

void TrailLayouter::buildGraph(
	std::vector<std::shared_ptr<DummyNode>>& dummyNodes,
	const std::vector<std::shared_ptr<DummyEdge>>& dummyEdges,
	const std::map<Id, Id>& topLevelAncestorIds)
{
	for (const std::shared_ptr<DummyNode> dummyNode : dummyNodes)
	{
		addNode(dummyNode);
	}

	for (const std::shared_ptr<DummyEdge> dummyEdge : dummyEdges)
	{
		dummyEdge->path.clear();

		if (dummyEdge->data && !dummyEdge->data->isType(Edge::EDGE_OVERRIDE | Edge::EDGE_INHERITANCE))
		{
			addEdge(dummyEdge, topLevelAncestorIds);
		}
	}

	for (const std::shared_ptr<DummyEdge> dummyEdge : dummyEdges)
	{
		if (dummyEdge->data && dummyEdge->data->isType(Edge::EDGE_OVERRIDE | Edge::EDGE_INHERITANCE))
		{
			addEdge(dummyEdge, topLevelAncestorIds);
		}
	}
}

void TrailLayouter::makeAcyclic()
{
	std::set<TrailEdge*> edgesToSwitch;
	for (std::shared_ptr<TrailNode> node : m_allNodes)
	{
		if (!node->incomingEdges.size())
		{
			utility::append(edgesToSwitch, node->outgoingEdges);
		}
	}

	for (TrailEdge* edge : edgesToSwitch)
	{
		switchEdge(edge);
	}

	for (TrailNode* node : m_rootNodes)
	{
		std::set<TrailEdge*> edgesToSwitch = node->incomingEdges;
		for (TrailEdge* edge : edgesToSwitch)
		{
			switchEdge(edge);
		}

		makeAcyclicRecursive(node, std::set<TrailNode*>());
	}
}

void TrailLayouter::makeAcyclicRecursive(TrailNode* node, std::set<TrailNode*> predecessors)
{
	predecessors.insert(node);

	std::set<TrailEdge*> edgesToSwitch;
	for (TrailEdge* edge : node->outgoingEdges)
	{
		if (predecessors.find(edge->target) != predecessors.end())
		{
			edgesToSwitch.insert(edge);
		}
		else
		{
			makeAcyclicRecursive(edge->target, predecessors);
		}
	}

	for (TrailEdge* edge : edgesToSwitch)
	{
		switchEdge(edge);
	}
}

void TrailLayouter::assignLongestPathLevels()
{
	std::set<TrailNode*> nodes(m_rootNodes.begin(), m_rootNodes.end());
	std::map<TrailNode*, TrailNode*> predecessorNodes;

	int level = 0;

	while (nodes.size())
	{
		std::set<TrailNode*> newNodes;

		for (TrailNode* node : nodes)
		{
			for (TrailEdge* edge : node->outgoingEdges)
			{
				newNodes.insert(edge->target);
				predecessorNodes[edge->target] = node;
			}
		}

		if (!newNodes.size())
		{
			break;
		}

		nodes = newNodes;
		level++;
	}

	while (nodes.size())
	{
		std::set<TrailNode*> newNodes;

		for (TrailNode* node : nodes)
		{
			node->level = level;

			if (level > 0)
			{
				newNodes.insert(predecessorNodes[node]);
			}
		}

		nodes = newNodes;
		level--;
	}
}

void TrailLayouter::assignRemainingLevels()
{
	std::set<TrailNode*> nodes(m_rootNodes.begin(), m_rootNodes.end());
	while (nodes.size())
	{
		std::set<TrailNode*> newNodes;

		for (TrailNode* node : nodes)
		{
			for (TrailEdge* edge : node->outgoingEdges)
			{
				newNodes.insert(edge->target);
			}

			if (node->level < 0)
			{
				int level = -1;

				for (TrailEdge* edge : node->incomingEdges)
				{
					level = std::max(level, edge->origin->level + 1);
				}

				node->level = level;
			}
		}

		nodes = newNodes;
	}
}

void TrailLayouter::addVirtualNodes()
{
	std::vector<std::shared_ptr<TrailEdge>> newEdges;

	for (std::shared_ptr<TrailEdge> edge : m_allEdges)
	{
		for (int i = edge->origin->level + 1; i < edge->target->level; i++)
		{
			std::shared_ptr<TrailNode> virtualNode = std::make_shared<TrailNode>();
			virtualNode->id = 0;
			virtualNode->name = "<virtual>";
			virtualNode->dummyNode = nullptr;
			virtualNode->level = i;

			virtualNode->size = Vec2i(50, 20);

			m_allNodes.push_back(virtualNode);
			edge->virtualNodes.push_back(virtualNode.get());


			std::shared_ptr<TrailEdge> virtualEdge = std::make_shared<TrailEdge>();
			virtualEdge->id = 0;

			virtualEdge->origin = edge->origin;
			virtualEdge->origin->outgoingEdges.erase(edge.get());
			virtualEdge->origin->outgoingEdges.insert(virtualEdge.get());

			virtualEdge->target = virtualNode.get();
			virtualEdge->target->incomingEdges.insert(virtualEdge.get());
			virtualEdge->target->outgoingEdges.insert(edge.get());

			edge->origin = virtualNode.get();
			newEdges.push_back(virtualEdge);
		}
	}

	m_allEdges.insert(m_allEdges.end(), newEdges.begin(), newEdges.end());
}

void TrailLayouter::buildColumns()
{
	for (std::shared_ptr<TrailNode> node : m_allNodes)
	{
		int level = node->level + 1;
		for (int i = m_nodesPerCol.size(); i <= level; i++)
		{
			m_nodesPerCol.push_back(std::vector<TrailNode*>());
		}

		m_nodesPerCol[level].push_back(node.get());
	}
}

void TrailLayouter::reduceEdgeCrossings()
{
	for (size_t i = 1; i < m_nodesPerCol.size(); i++)
	{
		std::vector<TrailNode*> nodes = m_nodesPerCol[i];

		bool usePredecessors = true;
		std::vector<TrailNode*> neighbors = m_nodesPerCol[i - 1];
		if (neighbors.size() == 1 && i + 1 < m_nodesPerCol.size() && m_nodesPerCol[i + 1].size() > 0)
		{
			neighbors = m_nodesPerCol[i + 1];
			usePredecessors = false;
		}

		std::map<TrailNode*, size_t> neighborIndices;
		for (size_t j = 0; j < neighbors.size(); j++)
		{
			neighborIndices.emplace(neighbors[j], j);
		}

		std::multimap<float, TrailNode*> newOrder;

		for (size_t j = 0; j < nodes.size(); j++)
		{
			TrailNode* node = nodes[j];

			size_t sum = 0;
			size_t count = 0;

			if (usePredecessors)
			{
				for (TrailEdge* edge : node->incomingEdges)
				{
					sum += neighborIndices[edge->origin];
					count++;
				}
			}
			else
			{
				for (TrailEdge* edge : node->outgoingEdges)
				{
					sum += neighborIndices[edge->target];
					count++;
				}
			}

			float value = j;
			if (count)
			{
				value = float(sum) / count;
			}
			newOrder.emplace(value, node);
		}

		nodes.clear();
		for (std::pair<float, TrailNode*> p : newOrder)
		{
			nodes.push_back(p.second);
		}
		m_nodesPerCol[i] = nodes;
	}
}

void TrailLayouter::layout()
{
	// calculate widths and heights of columns, and find largest column
	std::vector<int> widthsPerCol;
	std::vector<int> heightsPerCol;

	int maxHeight = 0;
	size_t maxHeightIndex = 0;

	unsigned int xIdx = horizontalLayout() ? 0 : 1;
	unsigned int yIdx = horizontalLayout() ? 1 : 0;

	for (size_t i = 0; i < m_nodesPerCol.size(); i++)
	{
		std::vector<TrailNode*>& nodes = m_nodesPerCol[i];

		int width = 0;
		int height = -30;

		for (TrailNode* node : nodes)
		{
			height += node->size.getValue(yIdx) + 30;
			width = std::max(width, node->size.getValue(xIdx));
		}

		widthsPerCol.push_back(width);
		heightsPerCol.push_back(height);

		if (height > maxHeight)
		{
			maxHeight = height;
			maxHeightIndex = i;
		}
	}

	// layout nodes within columns
	int x = 0;
	for (size_t i = 0; i < m_nodesPerCol.size(); i++)
	{
		std::vector<TrailNode*>& nodes = m_nodesPerCol[i];
		int y = -heightsPerCol[i] / 2;

		for (TrailNode* node : nodes)
		{
			node->pos = horizontalLayout() ? Vec2i(x, y) : Vec2i(y, x);
			y += node->size.getValue(yIdx) + 30;

			if (!node->id)
			{
				node->size.setValue(xIdx, widthsPerCol[i]);
			}
		}

		if (i + 1 == m_nodesPerCol.size())
		{
			continue;
		}

		if (invertedLayout())
		{
			x -= widthsPerCol[i + 1] + 150;
		}
		else
		{
			x += widthsPerCol[i] + 150;
		}
	}

	// process layout before highest column
	for (size_t i = maxHeightIndex; i > 0; i--)
	{
		moveNodesToAveragePosition(m_nodesPerCol[i - 1]);
	}

	// process layout after highest column
	for (size_t i = maxHeightIndex + 1; i < m_nodesPerCol.size(); i++)
	{
		moveNodesToAveragePosition(m_nodesPerCol[i]);
	}

	// put into grid
}

void TrailLayouter::moveNodesToAveragePosition(std::vector<TrailNode*> nodes)
{
	unsigned int yIdx = horizontalLayout() ? 1 : 0;

	for (size_t k = 0; k < 2; k++)
	{
		for (size_t j = 0; j < nodes.size(); j++)
		{
			size_t l = k ? nodes.size() - 1 - j : j;
			size_t i = l % 2 ? nodes.size() - (l + 1) / 2 : l / 2;
			TrailNode* node = nodes[i];

			int sum = 0;
			int count = 0;

			for (TrailEdge* edge : node->outgoingEdges)
			{
				sum += edge->target->pos.getValue(yIdx) + edge->target->size.getValue(yIdx) / 2;
				count++;
			}

			for (TrailEdge* edge : node->incomingEdges)
			{
				sum += edge->origin->pos.getValue(yIdx) + edge->origin->size.getValue(yIdx) / 2;
				count++;
			}

			if (count)
			{
				node->pos.setValue(yIdx, sum / count - node->size.getValue(yIdx) / 2);

				TrailNode* above = i > 0 ? nodes[i - 1] : nullptr;
				if (above && above->pos.getValue(yIdx) + above->size.getValue(yIdx) + 30 > node->pos.getValue(yIdx))
				{
					node->pos.setValue(yIdx, above->pos.getValue(yIdx) + above->size.getValue(yIdx) + 30);
				}

				TrailNode* below = i + 1 < nodes.size() ? nodes[i + 1] : nullptr;
				if (below && below->pos.getValue(yIdx) - 30 < node->pos.getValue(yIdx) + node->size.getValue(yIdx))
				{
					node->pos.setValue(yIdx, below->pos.getValue(yIdx) - 30 - node->size.getValue(yIdx));
				}
			}
		}
	}
}

void TrailLayouter::retrievePositions(const std::map<Id, Id>& topLevelAncestorIds)
{
	for (std::shared_ptr<TrailNode> node : m_allNodes)
	{
		if (node->dummyNode)
		{
			node->dummyNode->position = node->pos;
		}
	}

	for (std::shared_ptr<TrailEdge> edge : m_allEdges)
	{
		if (edge->virtualNodes.size())
		{
			for (DummyEdge* dummyEdge : edge->dummyEdges)
			{
				bool forward = edge->target->id == topLevelAncestorIds.find(dummyEdge->targetId)->second;
				for (size_t i = 0; i < edge->virtualNodes.size(); i++)
				{
					TrailNode* node = edge->virtualNodes[forward ? i : edge->virtualNodes.size() - 1 - i];
					dummyEdge->path.push_back(
						Vec4i(node->pos.x, node->pos.y, node->pos.x + node->size.x, node->pos.y + node->size.y));
				}
			}
		}
	}
}

void TrailLayouter::print()
{
	std::cout << "graph: " << std::endl;
	for (std::shared_ptr<TrailNode> node : m_allNodes)
	{
		std::cout << node->id << "\t" << node->level << "\t";
		std::cout << node->incomingEdges.size() << "\t" << node->outgoingEdges.size() << "\t";
		std::cout << node->name << std::endl;
	}
	std::cout << std::endl;

	for (std::shared_ptr<TrailEdge> edge : m_allEdges)
	{
		std::cout << edge->id << "\t" << edge->origin->name << "\t" << edge->target->name << std::endl;
	}
	std::cout << std::endl;
}

void TrailLayouter::addNode(const std::shared_ptr<DummyNode>& dummyNode)
{
	std::shared_ptr<TrailNode> node = std::make_shared<TrailNode>();
	node->id = dummyNode->tokenId;
	node->name = dummyNode->name;
	node->dummyNode = dummyNode.get();
	node->level = -1;

	node->size = dummyNode->size;

	m_allNodes.push_back(node);

	if (node->id)
	{
		m_nodesById.emplace(node->id, node.get());
	}

	if (dummyNode->hasActiveSubNode())
	{
		m_rootNodes.push_back(node.get());
	}
}

void TrailLayouter::addEdge(const std::shared_ptr<DummyEdge> dummyEdge, const std::map<Id, Id>& topLevelAncestorIds)
{
	std::shared_ptr<TrailEdge> edge = std::make_shared<TrailEdge>();
	if (!dummyEdge->data)
	{
		return;
	}

	edge->id = dummyEdge->data->getId();

	Id originTopLevelId = topLevelAncestorIds.find(dummyEdge->ownerId)->second;
	Id targetTopLevelId = topLevelAncestorIds.find(dummyEdge->targetId)->second;

	if (dummyEdge->data->isType(Edge::EDGE_OVERRIDE | Edge::EDGE_INHERITANCE) != invertedLayout())
	{
		std::swap(originTopLevelId, targetTopLevelId);
	}

	auto origin = m_nodesById.find(originTopLevelId);
	auto target = m_nodesById.find(targetTopLevelId);

	if (origin == m_nodesById.end() || target == m_nodesById.end() || origin == target)
	{
		return;
	}

	edge->origin = origin->second;
	edge->target = target->second;

	for (std::shared_ptr<TrailEdge> e : m_allEdges)
	{
		if ((e->origin == edge->origin && e->target == edge->target) ||
			(e->origin == edge->target && e->target == edge->origin))
		{
			e->dummyEdges.push_back(dummyEdge.get());
			return;
		}
	}

	edge->dummyEdges.push_back(dummyEdge.get());

	edge->origin->outgoingEdges.insert(edge.get());
	edge->target->incomingEdges.insert(edge.get());

	m_allEdges.push_back(edge);
}

void TrailLayouter::switchEdge(TrailEdge* edge)
{
	edge->origin->outgoingEdges.erase(edge);
	edge->origin->incomingEdges.insert(edge);

	edge->target->incomingEdges.erase(edge);
	edge->target->outgoingEdges.insert(edge);

	std::swap(edge->origin, edge->target);
}

bool TrailLayouter::horizontalLayout() const
{
	return m_direction == LAYOUT_LEFT_RIGHT || m_direction == LAYOUT_RIGHT_LEFT;
}

bool TrailLayouter::invertedLayout() const
{
	return m_direction == LAYOUT_RIGHT_LEFT || m_direction == LAYOUT_BOTTOM_TOP;
}
