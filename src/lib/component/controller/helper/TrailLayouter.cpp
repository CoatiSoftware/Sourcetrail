#include "TrailLayouter.h"

#include <iostream>

TrailLayouter::TrailLayouter(LayoutDirection dir): m_direction(dir), m_rootNode(nullptr) {}

void TrailLayouter::layoutGraph(
	std::vector<std::shared_ptr<DummyNode>>& dummyNodes,
	const std::vector<std::shared_ptr<DummyEdge>>& dummyEdges,
	const std::map<Id, Id>& topLevelAncestorIds)
{
	buildGraph(dummyNodes, dummyEdges, topLevelAncestorIds);

	if (!m_rootNode)
	{
		return;
	}

	removeDeadEnds();
	makeAcyclicRecursive(m_rootNode, std::set<TrailNode*>());

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
	for (const std::shared_ptr<DummyNode>& dummyNode: dummyNodes)
	{
		if (dummyNode->visible)
		{
			addNode(dummyNode);
		}
	}

	for (const std::shared_ptr<DummyEdge>& dummyEdge: dummyEdges)
	{
		dummyEdge->path.clear();

		if (dummyEdge->visible)
		{
			addEdge(dummyEdge, topLevelAncestorIds);
		}
	}
}

void TrailLayouter::removeDeadEnds()
{
	std::set<TrailNode*> predecessors;

	std::set<TrailNode*> deadEnds;
	std::set<TrailNode*> loseEnds;

	std::deque<TrailNode*> nodes;
	nodes.push_back(m_rootNode);

	while (nodes.size())
	{
		TrailNode* node = nodes.front();
		nodes.pop_front();

		if (predecessors.find(node) == predecessors.end())
		{
			predecessors.insert(node);

			for (TrailEdge* edge: node->outgoingEdges)
			{
				if (predecessors.find(edge->target) == predecessors.end())
				{
					nodes.push_back(edge->target);
				}
			}

			for (TrailEdge* edge: node->incomingEdges)
			{
				if (predecessors.find(edge->origin) == predecessors.end())
				{
					loseEnds.insert(edge->origin);
				}
			}

			if (!node->outgoingEdges.size())
			{
				deadEnds.insert(node);
			}
		}

		while (!nodes.size() && (deadEnds.size() || loseEnds.size()) &&
			   predecessors.size() < m_allNodes.size())
		{
			if (deadEnds.size())
			{
				TrailNode* deadEnd = *deadEnds.begin();
				deadEnds.erase(deadEnds.begin());

				for (TrailEdge* edge: deadEnd->incomingEdges)
				{
					if (predecessors.find(edge->origin) == predecessors.end())
					{
						nodes.push_back(edge->origin);
						switchEdge(edge);
						break;
					}
				}
			}
			else
			{
				TrailNode* loseEnd = *loseEnds.begin();
				loseEnds.erase(loseEnds.begin());

				if (predecessors.find(loseEnd) == predecessors.end())
				{
					for (TrailEdge* edge: loseEnd->outgoingEdges)
					{
						if (predecessors.find(edge->target) != predecessors.end())
						{
							nodes.push_back(loseEnd);
							switchEdge(edge);
							break;
						}
					}
				}
			}
		}
	}
}

void TrailLayouter::makeAcyclicRecursive(TrailNode* node, std::set<TrailNode*> predecessors)
{
	predecessors.insert(node);

	std::set<TrailEdge*> edgesToSwitch;
	for (TrailEdge* edge: node->outgoingEdges)
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

	for (TrailEdge* edge: edgesToSwitch)
	{
		switchEdge(edge);
	}
}

void TrailLayouter::assignLongestPathLevels()
{
	std::set<TrailNode*> nodes;
	nodes.insert(m_rootNode);

	std::map<TrailNode*, TrailNode*> predecessorNodes;

	int level = 0;

	while (nodes.size())
	{
		std::set<TrailNode*> newNodes;

		for (TrailNode* node: nodes)
		{
			for (TrailEdge* edge: node->outgoingEdges)
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

		for (TrailNode* node: nodes)
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
	std::multimap<int, TrailNode*> nodes;
	nodes.emplace(m_rootNode->level, m_rootNode);

	std::set<TrailNode*> allNodes;
	allNodes.insert(m_rootNode);

	while (nodes.size())
	{
		std::multimap<int, TrailNode*> newNodes;

		for (std::pair<int, TrailNode*> p: nodes)
		{
			TrailNode* node = p.second;

			for (TrailEdge* edge: node->outgoingEdges)
			{
				if (allNodes.insert(edge->target).second)
				{
					newNodes.emplace(edge->target->level, edge->target);
				}
			}

			if (node->level < 0)
			{
				int level = -1;

				for (TrailEdge* edge: node->incomingEdges)
				{
					if (edge->origin->level == -1)
					{
						if (allNodes.insert(edge->origin).second)
						{
							newNodes.emplace(edge->origin->level, edge->origin);
						}

						level = node->level;
						newNodes.emplace(level, node);
						break;
					}
					else
					{
						level = std::max(level, edge->origin->level + 1);
					}
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

	for (const std::shared_ptr<TrailEdge>& edge: m_allEdges)
	{
		for (int i = edge->origin->level + 1; i < edge->target->level; i++)
		{
			std::shared_ptr<TrailNode> virtualNode = std::make_shared<TrailNode>();
			virtualNode->id = 0;
			virtualNode->name = L"<virtual>";
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
	for (const std::shared_ptr<TrailNode>& node: m_allNodes)
	{
		const int level = node->level + 1;
		for (int i = static_cast<int>(m_nodesPerCol.size()); i <= level; i++)
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
				for (TrailEdge* edge: node->incomingEdges)
				{
					sum += neighborIndices[edge->origin];
					count++;
				}
			}
			else
			{
				for (TrailEdge* edge: node->outgoingEdges)
				{
					sum += neighborIndices[edge->target];
					count++;
				}
			}

			float value = float(j);
			if (count)
			{
				value = float(sum) / count;
			}
			newOrder.emplace(value, node);
		}

		nodes.clear();
		for (std::pair<float, TrailNode*> p: newOrder)
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

		for (TrailNode* node: nodes)
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

		for (TrailNode* node: nodes)
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
		moveNodesToAveragePosition(m_nodesPerCol[i - 1], false);
	}

	// process layout after highest column
	for (size_t i = maxHeightIndex + 1; i < m_nodesPerCol.size(); i++)
	{
		moveNodesToAveragePosition(m_nodesPerCol[i], true);
	}

	// put into grid
}

void TrailLayouter::moveNodesToAveragePosition(std::vector<TrailNode*> nodes, bool forward)
{
	unsigned int yIdx = horizontalLayout() ? 1 : 0;

	std::map<int, std::vector<TrailNode*>> averagePositions;
	for (TrailNode* node: nodes)
	{
		int sum = 0;
		int count = 0;

		if ((forward && node->incomingEdges.size()) || (!forward && !node->outgoingEdges.size()))
		{
			for (TrailEdge* edge: node->incomingEdges)
			{
				sum += edge->origin->pos.getValue(yIdx) + edge->origin->size.getValue(yIdx) / 2;
				count++;
			}
		}
		else
		{
			for (TrailEdge* edge: node->outgoingEdges)
			{
				sum += edge->target->pos.getValue(yIdx) + edge->target->size.getValue(yIdx) / 2;
				count++;
			}
		}

		if (count)
		{
			averagePositions[sum / count].push_back(node);
		}
	}

	if (!averagePositions.size())
	{
		return;
	}

	int averagePosition = 0;
	for (const std::pair<int, std::vector<TrailNode*>>& p: averagePositions)
	{
		averagePosition += p.first;
	}
	averagePosition /= static_cast<int>(averagePositions.size());


	std::multimap<int, int> distanceFromAveragePosition;
	for (std::pair<int, std::vector<TrailNode*>> p: averagePositions)
	{
		distanceFromAveragePosition.emplace(std::abs(averagePosition - p.first), p.first);
	}

	int currentTop = averagePosition;
	int currentBottom = averagePosition;

	for (std::pair<int, int> p: distanceFromAveragePosition)
	{
		int groupAveragePosition = p.second;
		std::vector<TrailNode*> nodeGroup = averagePositions.find(groupAveragePosition)->second;

		int size = -30;
		for (TrailNode* node: nodeGroup)
		{
			size += node->size.getValue(yIdx) + 30;
		}

		int top = groupAveragePosition - size / 2;
		if (currentTop != currentBottom)
		{
			if (top < currentTop)
			{
				if (top + size + 30 > currentTop)
				{
					top = currentTop - 30 - size;
				}
			}
			else if (top > currentBottom)
			{
				if (top - 30 < currentBottom)
				{
					top = currentBottom + 30;
				}
			}
			else
			{
				if ((currentTop + currentBottom) / 2 > top + size / 2)
				{
					top = currentTop - 30 - size;
				}
				else
				{
					top = currentBottom + 30;
				}
			}
		}

		int y = top;

		for (TrailNode* node: nodeGroup)
		{
			node->pos.setValue(yIdx, y);
			y += node->size.getValue(yIdx) + 30;
		}

		if (currentTop == currentBottom)
		{
			currentTop = top;
			currentBottom = top + size;
		}
		else if (top < currentTop)
		{
			currentTop = top;
		}
		else if (top + size > currentBottom)
		{
			currentBottom = top + size;
		}
	}
}

void TrailLayouter::retrievePositions(const std::map<Id, Id>& topLevelAncestorIds)
{
	for (std::shared_ptr<TrailNode>& node: m_allNodes)
	{
		if (node->dummyNode)
		{
			if (node->level != -1)
			{
				node->dummyNode->position = node->pos;
			}
			else
			{
				node->dummyNode->visible = false;
			}
		}
	}

	for (std::shared_ptr<TrailEdge>& edge: m_allEdges)
	{
		if (edge->virtualNodes.size())
		{
			for (DummyEdge* dummyEdge: edge->dummyEdges)
			{
				bool forward = edge->target->id ==
					topLevelAncestorIds.find(dummyEdge->targetId)->second;
				for (size_t i = 0; i < edge->virtualNodes.size(); i++)
				{
					TrailNode* node =
						edge->virtualNodes[forward ? i : edge->virtualNodes.size() - 1 - i];
					dummyEdge->path.push_back(Vec4i(
						node->pos.x,
						node->pos.y,
						node->pos.x + node->size.x,
						node->pos.y + node->size.y));
				}
			}
		}
	}
}

void TrailLayouter::print()
{
	std::cout << "graph: " << std::endl;
	for (std::shared_ptr<TrailNode>& node: m_allNodes)
	{
		if (node->id)
		{
			std::cout << node->id << "\t" << node->level << "\t";
			std::cout << node->incomingEdges.size() << "\t" << node->outgoingEdges.size() << "\t";
			std::wcout << node->name << std::endl;
		}
	}
	std::cout << std::endl;

	for (std::shared_ptr<TrailEdge>& edge: m_allEdges)
	{
		if (edge->origin->id || edge->target->id)
		{
			std::wcout << edge->id << L"\t" << edge->origin->name << L"\t" << edge->target->name
					   << std::endl;
		}
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

	if (!m_rootNode && dummyNode->hasActiveSubNode())
	{
		m_rootNode = node.get();
	}
}

void TrailLayouter::addEdge(
	const std::shared_ptr<DummyEdge> dummyEdge, const std::map<Id, Id>& topLevelAncestorIds)
{
	std::shared_ptr<TrailEdge> edge = std::make_shared<TrailEdge>();
	if (!dummyEdge->data)
	{
		return;
	}

	edge->id = dummyEdge->data->getId();

	Id originTopLevelId = topLevelAncestorIds.find(dummyEdge->ownerId)->second;
	Id targetTopLevelId = topLevelAncestorIds.find(dummyEdge->targetId)->second;

	if (dummyEdge->data->isType(Edge::LAYOUT_VERTICAL) != invertedLayout())
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

	for (std::shared_ptr<TrailEdge>& e: m_allEdges)
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
