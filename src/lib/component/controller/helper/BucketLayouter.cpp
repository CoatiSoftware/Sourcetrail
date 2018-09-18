#include "BucketLayouter.h"

#include "DummyEdge.h"
#include "GraphViewStyle.h"

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
	for (const std::shared_ptr<DummyNode>& n : m_nodes)
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

void Bucket::preLayout(Vec2i viewSize, bool addVerticalSplit, bool forceVerticalSplit)
{
	int cols = (viewSize.y > 0 ? (m_height / viewSize.y) : 0) + 1;

	int x = 0;
	int y = 0;
	int height = m_height / cols;
	int width = 0;

	m_height = 0;

	std::vector<int> colWidths;
	std::vector<int> colHeights;
	std::vector<std::vector<DummyNode*>> nodesInCol;
	nodesInCol.push_back({ });

	int heightDiff = 0;
	for (const std::shared_ptr<DummyNode>& node : m_nodes)
	{
		if (y > height + heightDiff)
		{
			// keep adding to the same columns if it only contains 1 element, which will end up above the middle split
			if (nodesInCol.back().size() == 1 && m_nodes.size() > 1 && addVerticalSplit | forceVerticalSplit)
			{
				heightDiff = y;
			}
			else
			{
				colHeights.push_back(y - GraphViewStyle::s_gridCellPadding);
				colWidths.push_back(width);

				y = 0;
				x += GraphViewStyle::toGridOffset(width + 45);
				width = 0;

				nodesInCol.push_back({ });

				heightDiff = 0;
			}
		}

		node->position.x = x;
		node->position.y = y;

		nodesInCol.back().push_back(node.get());

		y += GraphViewStyle::toGridSize(node->size.y) + GraphViewStyle::s_gridCellPadding;

		width = std::max(width, node->size.x());
		m_height = std::max(m_height, y);
	}

	colHeights.push_back(y - GraphViewStyle::s_gridCellPadding);
	colWidths.push_back(width);

	m_width = x + width;
	m_height -= GraphViewStyle::s_gridCellPadding;

	for (size_t i = 0; i < nodesInCol.size(); i++)
	{
		for (DummyNode* node : nodesInCol[i])
		{
			node->columnSize.x = colWidths[i];
			node->columnSize.y = colHeights[i];
		}
	}

	addVerticalSplit &= nodesInCol.size() > 1 || forceVerticalSplit;
	if (!addVerticalSplit)
	{
		return;
	}

	// align each column vertically and leave a gap in the middle where edges can pass through
	// NOTE: m_height is not gonna be correct after this, but stays unchanged to allow correct positioning next to
	// active node.
	int nodeOffset = GraphViewStyle::s_gridCellPadding + GraphViewStyle::s_gridCellSize;

	for (size_t i = 0; i < nodesInCol.size(); i++)
	{
		int offset = 0;
		bool hasOffset = false;
		int mid = colHeights[i] / 2;

		std::vector<DummyNode*> aboveNodes;
		std::vector<DummyNode*> belowNodes;

		int aboveNodesMaxWidth = 0;
		int belowNodesMaxWidth = 0;

		for (DummyNode* node : nodesInCol[i])
		{
			bool above = true;

			if (hasOffset)
			{
				above = false;
			}
			else if (nodesInCol[i].size() == 1)
			{
				offset -= (node->size.y + GraphViewStyle::s_gridCellPadding) / 2;
			}
			else if (node->position.y < mid && node->position.y + node->size.y > mid)
			{
				if (mid - node->position.y < (node->position.y + node->size.y) - mid)
				{
					offset = mid - node->position.y + GraphViewStyle::s_gridCellPadding / 2;
					above = false;
				}
				else
				{
					offset = mid - (node->position.y + node->size.y) - GraphViewStyle::s_gridCellPadding / 2;
				}
				hasOffset = true;
			}
			else if (node->position.y + node->size.y < mid &&
				mid < node->position.y + node->size.y + GraphViewStyle::s_gridCellPadding)
			{
				offset = mid - (node->position.y + node->size.y + GraphViewStyle::s_gridCellPadding / 2);
				hasOffset = true;
			}

			if (above)
			{
				aboveNodes.push_back(node);
				aboveNodesMaxWidth = std::max(aboveNodesMaxWidth, node->size.x());
			}
			else
			{
				belowNodes.push_back(node);
				belowNodesMaxWidth = std::max(belowNodesMaxWidth, node->size.x());
			}
		}

		offset += (m_height - colHeights[i]) / 2;
		for (DummyNode* node : aboveNodes)
		{
			node->position.y() += offset - nodeOffset;
			node->columnSize.x() = aboveNodesMaxWidth;
		}
		for (DummyNode* node : belowNodes)
		{
			node->position.y() += offset + nodeOffset;
			node->columnSize.x() = belowNodesMaxWidth;
		}
	}
}

void Bucket::layout(int x, int y, int width, int height)
{
	if (!m_nodes.size())
	{
		return;
	}

	Vec2i offset = Vec2i(x + (width - m_width) / 2, y + (height - m_height) / 2);
	offset = GraphViewStyle::alignOnRaster((*m_nodes.begin())->position + offset) - (*m_nodes.begin())->position;

	for (const std::shared_ptr<DummyNode>& node : m_nodes)
	{
		node->position += offset;
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
	for (std::shared_ptr<DummyNode>& node : nodes)
	{
		if (node->hasActiveSubNode() || !edges.size())
		{
			addNode(node);
			node->bundleInfo.layoutVertical = false;
			activeNodeAdded = true;
			m_activeParentNode = node.get();
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

	for (std::shared_ptr<DummyEdge> edge : edges)
	{
		std::shared_ptr<DummyNode> owner = findTopMostDummyNodeRecursive(nodes, edge->ownerId, nullptr);
		std::shared_ptr<DummyNode> target = findTopMostDummyNodeRecursive(nodes, edge->targetId, nullptr);

		bool horizontal = true;

		if (owner && target && owner != target && owner->getsLayouted() && target->getsLayouted())
		{
			horizontal = !owner->bundleInfo.layoutVertical && !target->bundleInfo.layoutVertical;

			if (!horizontal)
			{
				if ((owner->bundleInfo.layoutVertical && owner->bundleInfo.isReferenced) ||
					(target->bundleInfo.layoutVertical && target->bundleInfo.isReferencing))
				{
					std::swap(owner, target);
				}
			}
			else if (edge->getDirection() == TokenComponentAggregation::DIRECTION_BACKWARD ||
				// put nodes with bidirectional edges on the left
				(edge->getDirection() == TokenComponentAggregation::DIRECTION_NONE &&
					!target->bundleInfo.isReferencing && !target->bundleInfo.isReferenced))
			{
				std::swap(owner, target);
			}

			horizontal = addNode(owner, target, horizontal);
		}

		edge->layoutHorizontal = horizontal;
	}
}

void BucketLayouter::layoutBuckets(bool addVerticalSplit)
{
	std::map<int, int> widths;
	std::map<int, int> heights;

	for (int j = m_j1; j <= m_j2; j++)
	{
		for (int i = m_i1; i <= m_i2; i++)
		{
			Bucket* bucket = &m_buckets[j][i];

			bucket->preLayout(m_viewSize, i != 0, addVerticalSplit);

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

	int verticalOffset = 0;
	if (m_activeParentNode)
	{
		Vec4i rect = m_activeParentNode->getActiveSubNodeRect();
		verticalOffset = (rect.y + rect.w - m_activeParentNode->size.y) / 2;
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
			// align buckets left and right of center to be vertically centered next to the active node
			else if (j == 0 && i != 0 && verticalOffset != 0)
			{
				yOff = verticalOffset;
			}

			bucket->layout(x, y + yOff, widths[i], heights[j]);
			x += widths[i] + GraphViewStyle::toGridGap(110);
		}

		y += heights[j] + GraphViewStyle::toGridGap(70);
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
	for (const std::shared_ptr<DummyNode>& node : nodes)
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
	Bucket* bucket = getBucket(0, 0);
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
		return ownerBucket->j == targetBucket->j;
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

	return horizontal;
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
