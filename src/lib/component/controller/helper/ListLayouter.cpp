#include "ListLayouter.h"

#include <algorithm>

#include "DummyNode.h"
#include "GraphViewStyle.h"

void ListLayouter::layoutRow(std::vector<std::shared_ptr<DummyNode>>* nodes, int gap)
{
	layoutSimple(nodes, gap, 0, true);
}

void ListLayouter::layoutColumn(std::vector<std::shared_ptr<DummyNode>>* nodes, int gap)
{
	layoutSimple(nodes, 0, gap, false);
}

void ListLayouter::layoutMultiColumn(Vec2i viewSize, std::vector<std::shared_ptr<DummyNode>>* nodes)
{
	size_t colsFinal;
	std::vector<int> maxWidthsFinal;

	int gapX = GraphViewStyle::s_gridCellSize + 2 * GraphViewStyle::s_gridCellPadding;
	int gapY = GraphViewStyle::s_gridCellPadding;

	std::vector<std::shared_ptr<DummyNode>> visibleNodes;
	for (auto node: *nodes)
	{
		if (node->getsLayouted())
		{
			visibleNodes.push_back(node);
		}
	}

	for (size_t cols = 1; cols <= 10; cols++)
	{
		std::vector<int> maxWidths = std::vector<int>(cols, 0);
		size_t nodesPerCol =
			(cols == 1
				 ? visibleNodes.size()
				 : static_cast<size_t>(std::ceil((visibleNodes.size() + cols - 1) / double(cols))));

		int maxHeight = 0;
		int height = -gapY;

		for (size_t i = 0; i < visibleNodes.size(); i++)
		{
			size_t j = i / nodesPerCol;

			if (i % nodesPerCol == 0)
			{
				height = -gapY;
			}
			height += visibleNodes[i]->size.y() + gapY;

			maxWidths[j] = std::max(visibleNodes[i]->size.x(), maxWidths[j]);
			maxHeight = std::max(height, maxHeight);
		}

		int width = -gapX;
		for (size_t j = 0; j < cols; j++)
		{
			width += maxWidths[j] + gapX;
		}

		if (width > viewSize.x)
		{
			if (!maxWidthsFinal.size())
			{
				colsFinal = 1;
				maxWidthsFinal = maxWidths;
			}
			break;
		}

		colsFinal = cols;
		maxWidthsFinal = maxWidths;

		if (height < viewSize.y)
		{
			break;
		}
	}

	int x = 0;
	int y = 0;

	size_t nodesPerCol =
		(colsFinal == 1 ? visibleNodes.size()
						: static_cast<size_t>(
							  std::ceil((visibleNodes.size() + colsFinal - 1) / double(colsFinal))));
	std::shared_ptr<DummyNode> lastTextNode;

	for (size_t i = 0; i < visibleNodes.size(); i++)
	{
		size_t j = i / nodesPerCol;
		if (j != 0 && j != colsFinal && i % nodesPerCol == 0)
		{
			if (lastTextNode && !visibleNodes[i]->isTextNode())
			{
				std::shared_ptr<DummyNode> textNode = std::make_shared<DummyNode>(*lastTextNode.get());

				if (visibleNodes[i - 1] == lastTextNode)
				{
					lastTextNode->visible = false;
				}
				else if (textNode->name.size() == 1)
				{
					textNode->name += L"..";
				}

				visibleNodes.insert(visibleNodes.begin() + i, textNode);
				nodes->push_back(textNode);
				lastTextNode.reset();
				i--;
				continue;
			}

			y = 0;
			x += maxWidthsFinal[j - 1] + gapX;
		}

		visibleNodes[i]->position.x = x;
		visibleNodes[i]->position.y = y;

		y += visibleNodes[i]->size.y + gapY;

		if (visibleNodes[i]->isTextNode())
		{
			lastTextNode = visibleNodes[i];
		}
	}
}

void ListLayouter::layoutSquare(std::vector<std::shared_ptr<DummyNode>>* nodes, int maxWidth)
{
	int gapX = GraphViewStyle::s_gridCellSize + 2 * GraphViewStyle::s_gridCellPadding;
	int gapY = GraphViewStyle::s_gridCellPadding;

	std::vector<std::shared_ptr<DummyNode>> visibleNodes;
	for (auto node: *nodes)
	{
		if (node->getsLayouted())
		{
			visibleNodes.push_back(node);
		}
	}

	int totalHeight = 0;
	for (size_t i = 0; i < visibleNodes.size(); i++)
	{
		totalHeight += visibleNodes[i]->size.y() + gapY;
	}

	int diff = -1;
	int cols = 1;

	for (int i = cols; i < 100; i++)
	{
		if (layoutSquareInternal(
				visibleNodes, Vec2i(maxWidth, totalHeight * i / 100), Vec2i(gapX, gapY)))
		{
			Vec4i rect = boundingRect(visibleNodes);

			int newDiff = rect.z() * rect.w() + (rect.z() - rect.w()) * (rect.z() - rect.w()) / 4;
			if (maxWidth >= 0)
			{
				newDiff = rect.w();
			}

			if (diff < 0 || newDiff <= diff)
			{
				diff = newDiff;
				cols = i;
			}
		}
	}

	layoutSquareInternal(visibleNodes, Vec2i(maxWidth, totalHeight * cols / 100), Vec2i(gapX, gapY));
}

bool ListLayouter::layoutSquareInternal(
	std::vector<std::shared_ptr<DummyNode>>& visibleNodes, const Vec2i& maxSize, const Vec2i& gap)
{
	int x = 0;
	int y = 0;

	int width = 0;

	for (std::shared_ptr<DummyNode> node: visibleNodes)
	{
		node->position.x() = x;
		node->position.y() = y;

		y += node->size.y() + gap.y();
		width = std::max(width, node->size.x());

		if (maxSize.x > 0 && x + width > maxSize.x)
		{
			return false;
		}

		if (y >= maxSize.y)
		{
			y = 0;
			x += width + gap.x();

			width = 0;
		}
	}

	return true;
}

void ListLayouter::layoutSkewed(
	std::vector<std::shared_ptr<DummyNode>>* nodes, int gapX, int gapY, int maxWidth)
{
	std::vector<std::shared_ptr<DummyNode>> visibleNodes;
	std::multiset<int> nodeWidths;
	for (auto node: *nodes)
	{
		if (node->getsLayouted())
		{
			visibleNodes.push_back(node);
			nodeWidths.insert(node->size.x());
		}
	}

	int nodeWidth = nodeWidths.size() ? *nodeWidths.rbegin() : 0;
	if (nodeWidths.size() > 1)
	{
		nodeWidth = *nodeWidths.rbegin() / 2 + *(++nodeWidths.rbegin()) / 2;
	}

	int height = 0;
	int width = 0;

	int nodesPerRowStart = std::max(int(std::floor(maxWidth * 2 / (nodeWidth + gapX))), 3);
	for (int nodesPerRow = nodesPerRowStart; nodesPerRow >= 3; nodesPerRow--)
	{
		height = 0;
		width = (nodeWidth * nodesPerRow + gapX * (nodesPerRow - 1)) / 2;

		int x = 0;
		int rowHeight = 0;
		int nodeCount = 0;
		bool evenRow = true;

		for (size_t i = 0; i < visibleNodes.size(); i++)
		{
			if (nodeCount + 2 > nodesPerRow)
			{
				height += rowHeight + gapY;
				rowHeight = 0;

				evenRow = !evenRow;
				nodeCount = evenRow ? 0 : 1;
				x = evenRow ? 0 : ((nodeWidth + gapX) / 2);
			}

			DummyNode* node = visibleNodes[i].get();
			node->position.x = x + (nodeWidth - node->size.x()) / 2;
			node->position.y = height;

			rowHeight = std::max(rowHeight, node->size.y());
			x += nodeWidth + gapX;

			nodeCount += 2;
		}

		height += rowHeight;

		if (height * 3 >= width)
		{
			break;
		}
	}
}

Vec4i ListLayouter::boundingRect(const std::vector<std::shared_ptr<DummyNode>>& nodes)
{
	Vec4i rect;

	for (auto node: nodes)
	{
		if (!node->getsLayouted())
		{
			continue;
		}

		if (rect.z() - rect.x() == 0)
		{
			rect.x = node->position.x();
			rect.y = node->position.y();
			rect.z = node->position.x() + node->size.x();
			rect.w = node->position.y() + node->size.y();
		}
		else
		{
			rect.x = std::min(rect.x(), node->position.x());
			rect.y = std::min(rect.y(), node->position.y());
			rect.z = std::max(rect.z(), node->position.x() + node->size.x());
			rect.w = std::max(rect.w(), node->position.y() + node->size.y());
		}
	}

	return rect;
}

Vec2i ListLayouter::offsetNodes(std::vector<std::shared_ptr<DummyNode>> nodes, int top, int left)
{
	Vec4i rect = boundingRect(nodes);
	Vec2i offset(left - rect.x(), top - rect.y());

	for (auto node: nodes)
	{
		if (node->getsLayouted())
		{
			node->position += offset;
		}
	}

	return Vec2i(rect.z() - rect.x(), rect.w() - rect.y());
}

void ListLayouter::layoutSimple(
	std::vector<std::shared_ptr<DummyNode>>* nodes, int gapX, int gapY, bool horizontal)
{
	int y = 0;
	int x = 0;

	for (const std::shared_ptr<DummyNode>& node: *nodes)
	{
		if (!node->getsLayouted())
		{
			continue;
		}

		node->position.x = x;
		node->position.y = y;

		if (horizontal)
		{
			x += node->size.x + gapX;
		}
		else
		{
			y += node->size.y + gapY;
		}
	}
}
