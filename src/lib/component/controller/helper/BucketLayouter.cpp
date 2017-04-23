#include "component/controller/helper/BucketLayouter.h"

#include "component/controller/helper/DummyEdge.h"
#include "component/view/GraphViewStyle.h"

Bucket::Bucket()
	: i(0)
	, j(0)
	, m_width(0)
	, m_height(0)
{
}

Bucket::Bucket(int i, int j)
	: i(i)
	, j(j)
	, m_width(0)
	, m_height(0)
{
}

int Bucket::getWidth() const
{
	return m_width;
}

int Bucket::getHeight() const
{
	return m_height;
}

bool Bucket::hasNode(std::shared_ptr<DummyNode> node) const
{
	for (std::shared_ptr<DummyNode> n : m_nodes)
	{
		if (node == n)
		{
			return true;
		}
	}

	return false;
}

void Bucket::addNode(std::shared_ptr<DummyNode> node)
{
	m_nodes.insert(node);

	m_width = (node->size.x > m_width ? node->size.x : m_width);
	m_height += GraphViewStyle::toGridSize(node->size.y) + GraphViewStyle::s_gridCellPadding;
}

const DummyNode::BundledNodesSet& Bucket::getNodes() const
{
	return m_nodes;
}

void Bucket::preLayout(Vec2i viewSize)
{
	int cols = (viewSize.y > 0 ? (m_height / viewSize.y) : 0) + 1;

	int x = 0;
	int y = 0;
	int height = m_height / cols;
	int width = 0;

	m_height = 0;

	for (std::shared_ptr<DummyNode> node : m_nodes)
	{
		node->position.x = x;
		node->position.y = y;

		y += GraphViewStyle::toGridSize(node->size.y) + GraphViewStyle::s_gridCellPadding;

		width = (node->size.x > width ? node->size.x : width);
		m_height = (y > m_height ? y : m_height);

		if (y > height)
		{
			y = 0;

			x += GraphViewStyle::toGridOffset(width + 30);
			width = 0;
		}
	}

	m_width = x + width;
}

void Bucket::layout(int x, int y, int width, int height)
{
	int cx = GraphViewStyle::toGridOffset(x + (width - m_width) / 2);
	int cy = GraphViewStyle::toGridOffset(y + (height - m_height) / 2);

	for (std::shared_ptr<DummyNode> node : m_nodes)
	{
		node->position.x = node->position.x + cx;
		node->position.y = node->position.y + cy;
	}
}


BucketLayouter::BucketLayouter(Vec2i viewSize)
	: m_viewSize(viewSize)
	, m_i1(0)
	, m_j1(0)
	, m_i2(0)
	, m_j2(0)
{
	m_buckets[0][0] = Bucket(0, 0);
}

void BucketLayouter::createBuckets(
	std::vector<std::shared_ptr<DummyNode>>& nodes, const std::vector<std::shared_ptr<DummyEdge>>& edges
){
	if (!nodes.size())
	{
		return;
	}

	bool activeNodeAdded = false;
	for (std::shared_ptr<DummyNode> node : nodes)
	{
		if (node->hasActiveSubNode() || !edges.size())
		{
			addNode(node);
			activeNodeAdded = true;
		}
	}

	if (!edges.size())
	{
		return;
	}

	if (!activeNodeAdded)
	{
		addNode(nodes[0]);
	}

	std::vector<const DummyEdge*> remainingEdges;
	for (std::shared_ptr<DummyEdge> edge : edges)
	{
		remainingEdges.push_back(edge.get());
	}

	size_t i = 0;
	while (remainingEdges.size())
	{
		const DummyEdge* edge = remainingEdges[i];

		std::shared_ptr<DummyNode> owner = findTopMostDummyNodeRecursive(nodes, edge->ownerId, nullptr);
		std::shared_ptr<DummyNode> target = findTopMostDummyNodeRecursive(nodes, edge->targetId, nullptr);

		bool removeEdge = false;
		if (!owner || !target || owner == target)
		{
			removeEdge = true;
		}
		else
		{
			bool horizontal = !owner->bundleInfo.layoutVertical && !target->bundleInfo.layoutVertical;

			if (!horizontal)
			{
				if ((owner->bundleInfo.layoutVertical && owner->bundleInfo.isReferenced) ||
					(target->bundleInfo.layoutVertical && target->bundleInfo.isReferencing))
				{
					std::swap(owner, target);
				}
			}
			else if (edge->getDirection() == TokenComponentAggregation::DIRECTION_BACKWARD)
			{
				std::swap(owner, target);
			}

			removeEdge = addNode(owner, target, horizontal);
		}

		if (removeEdge)
		{
			remainingEdges.erase(remainingEdges.begin() + i);
		}
		else
		{
			i++;
		}

		if (i == remainingEdges.size())
		{
			i = 0;
		}
	}
}

void BucketLayouter::layoutBuckets()
{
	std::map<int, int> widths;
	std::map<int, int> heights;

	for (int j = m_j1; j <= m_j2; j++)
	{
		for (int i = m_i1; i <= m_i2; i++)
		{
			Bucket* bucket = &m_buckets[j][i];

			bucket->preLayout(m_viewSize);

			std::map<int, int>::iterator wt = widths.find(i);
			if (wt == widths.end() || wt->second < bucket->getWidth())
			{
				widths[i] = bucket->getWidth();
			}

			std::map<int, int>::iterator ht = heights.find(j);
			if (ht == heights.end() || ht->second < bucket->getHeight())
			{
				heights[j] = bucket->getHeight();
			}
		}
	}

	int y = 0;
	for (int j = m_j1; j <= m_j2; j++)
	{
		int x = 0;
		for (int i = m_i1; i <= m_i2; i++)
		{
			Bucket* bucket = &m_buckets[j][i];

			int yOff = 0;
			// move buckets over or below the middle one closer
			if (i == 0 && (j == -1 || j == 1))
			{
				Bucket* midBucket = &m_buckets[0][0];
				yOff = (heights[0] - midBucket->getHeight()) / 2 * -j;
			}

			bucket->layout(x, y + yOff, widths[i], heights[j]);
			x += widths[i] + GraphViewStyle::toGridGap(85);
		}

		y += heights[j] + GraphViewStyle::toGridGap(45);
	}
}

std::vector<std::shared_ptr<DummyNode>> BucketLayouter::getSortedNodes()
{
	std::vector<std::shared_ptr<DummyNode>> sortedNodes;

	for (int j = m_j1; j <= m_j2; j++)
	{
		for (int i = m_i1; i <= m_i2; i++)
		{
			DummyNode::BundledNodesSet nodes = m_buckets[j][i].getNodes();
			sortedNodes.insert(sortedNodes.end(), nodes.begin(), nodes.end());
		}
	}

	return sortedNodes;
}

std::shared_ptr<DummyNode> BucketLayouter::findTopMostDummyNodeRecursive(
	std::vector<std::shared_ptr<DummyNode>>& nodes, Id tokenId, std::shared_ptr<DummyNode> top
){
	for (std::shared_ptr<DummyNode> node : nodes)
	{
		std::shared_ptr<DummyNode> t = (top ? top : node);

		if (node->visible && node->tokenId == tokenId)
		{
			return t;
		}

		std::shared_ptr<DummyNode> result = findTopMostDummyNodeRecursive(node->subNodes, tokenId, t);
		if (result != nullptr)
		{
			return result;
		}
	}

	return nullptr;
}

void BucketLayouter::addNode(std::shared_ptr<DummyNode> node)
{
	Bucket* bucket = getBucket(node->layoutBucket.x, node->layoutBucket.y);
	bucket->addNode(node);
}

bool BucketLayouter::addNode(std::shared_ptr<DummyNode> owner, std::shared_ptr<DummyNode> target, bool horizontal)
{
	Bucket* ownerBucket = getBucket(owner);
	Bucket* targetBucket = getBucket(target);

	if (!ownerBucket && !targetBucket)
	{
		ownerBucket = getBucket(0, 0);
		ownerBucket->addNode(owner);
	}
	else if (ownerBucket && targetBucket)
	{
		return true;
	}

	if (ownerBucket)
	{
		int i = horizontal ? ownerBucket->i + 1 : ownerBucket->i;
		int j = horizontal ? ownerBucket->j : ownerBucket->j - 1;

		Bucket* bucket = getBucket(i, j);
		bucket->addNode(target);
	}
	else
	{
		int i = horizontal ? targetBucket->i - 1 : targetBucket->i;
		int j = horizontal ? targetBucket->j : targetBucket->j + 1;

		Bucket* bucket = getBucket(i, j);
		bucket->addNode(owner);
	}

	return true;
}

Bucket* BucketLayouter::getBucket(int i, int j)
{
	bool newColumn = false;
	bool newRow = false;

	if (i == m_i1 - 1)
	{
		m_i1 = i;
		newColumn = true;
	}
	else if (i == m_i2 + 1)
	{
		m_i2 = i;
		newColumn = true;
	}

	if (newColumn)
	{
		for (int cj = m_j1; cj <= m_j2; cj++)
		{
			m_buckets[cj][i] = Bucket(i, cj);
		}
	}

	if (j == m_j1 - 1)
	{
		m_j1 = j;
		newRow = true;
	}
	else if (j == m_j2 + 1)
	{
		m_j2 = j;
		newRow = true;
	}

	if (newRow)
	{
		for (int ci = m_i1; ci <= m_i2; ci++)
		{
			m_buckets[j][ci] = Bucket(ci, j);
		}
	}

	if (i >= m_i1 && i <= m_i2 && j >= m_j1 && j <= m_j2)
	{
		return &m_buckets[j][i];
	}

	return nullptr;
}

Bucket* BucketLayouter::getBucket(std::shared_ptr<DummyNode> node)
{
	for (int j = m_j1; j <= m_j2; j++)
	{
		for (int i = m_i1; i <= m_i2; i++)
		{
			Bucket* bucket = &m_buckets[j][i];

			if (bucket->hasNode(node))
			{
				return bucket;
			}
		}
	}

	return nullptr;
}
