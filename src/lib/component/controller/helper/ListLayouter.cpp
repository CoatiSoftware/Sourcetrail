#include "component/controller/helper/ListLayouter.h"

#include <algorithm>

#include "component/controller/helper/DummyNode.h"
#include "component/view/GraphViewStyle.h"

ListLayouter::ListLayouter(Vec2i viewSize)
	: m_viewSize(viewSize)
{
}

void ListLayouter::layoutList(std::vector<std::shared_ptr<DummyNode>>& nodes)
{
	size_t colsFinal;
	std::vector<int> maxWidthsFinal;

	int gapX = GraphViewStyle::s_gridCellSize + 2 * GraphViewStyle::s_gridCellPadding;
	int gapY = GraphViewStyle::s_gridCellPadding;

	for (size_t cols = 1; cols <= 10; cols++)
	{
		std::vector<int> maxWidths = std::vector<int>(cols, 0);
		size_t nodesPerCol = cols == 1 ? nodes.size() : std::ceil((nodes.size() + cols - 1) / double(cols));
		int maxHeight = 0;
		int height = -gapY;

		for (size_t i = 0; i < nodes.size(); i++)
		{
			size_t j = i / nodesPerCol;

			if (i % nodesPerCol == 0)
			{
				height = -gapY;
			}
			height += nodes[i]->size.y() + gapY;

			maxWidths[j] = std::max(nodes[i]->size.x(), maxWidths[j]);
			maxHeight = std::max(height, maxHeight);
		}

		int width = -gapX;
		for (size_t j = 0; j < cols; j++)
		{
			width += maxWidths[j] + gapX;
		}

		if (width > m_viewSize.x)
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

		if (height < m_viewSize.y)
		{
			break;
		}
	}

	int x = 0;
	int y = 0;
	size_t nodesPerCol = colsFinal == 1 ? nodes.size() : std::ceil((nodes.size() + colsFinal - 1) / double(colsFinal));
	std::shared_ptr<DummyNode> lastTextNode;

	for (size_t i = 0; i < nodes.size(); i++)
	{
		size_t j = i / nodesPerCol;
		if (j != 0 && j != colsFinal && i % nodesPerCol == 0)
		{
			if (lastTextNode)
			{
				std::shared_ptr<DummyNode> textNode = std::make_shared<DummyNode>(*lastTextNode.get());

				if (nodes[i - 1] == lastTextNode)
				{
					lastTextNode->visible = false;
				}
				else if (textNode->name.size() == 1)
				{
					textNode->name += L"..";
				}

				nodes.insert(nodes.begin() + i, textNode);
				lastTextNode.reset();
				i--;
				continue;
			}

			y = 0;
			x += maxWidthsFinal[j - 1] + gapX;
		}

		nodes[i]->position.x = x;
		nodes[i]->position.y = y;

		y += nodes[i]->size.y + gapY;

		if (nodes[i]->isTextNode())
		{
			lastTextNode = nodes[i];
		}
	}
}
