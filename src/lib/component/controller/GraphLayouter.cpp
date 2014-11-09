#include "component/controller/GraphLayouter.h"

#include <cmath>

#include "component/view/graphElements/GraphNode.h"

void GraphLayouter::layoutSimpleRaster(std::vector<DummyNode>& nodes)
{
	int x = 0;
	int y = 0;
	int offset = 150;

	int w = ceil(sqrt(nodes.size()));

	for (unsigned int i = 0; i < nodes.size(); i++)
	{
		if (i > 0 && i % w == 0)
		{
			y += offset;
			x = 0;
		}

		nodes[i].position = Vec2i(x, y);

		x += offset;
	}
}

void GraphLayouter::layoutSimpleRing(std::vector<DummyNode>& nodes)
{
	if (nodes.size() >= 1)
	{
		nodes[0].position = Vec2i(0, 0);

		if (nodes.size() > 1)
		{
			float offset = 200.0f;

			for (unsigned int i = 1; i < nodes.size(); i++)
			{
				float rad = 2.0f * 3.14159265359f / float(nodes.size() - 1) * i - 1;

				int x = offset * std::cos(rad);
				int y = offset * std::sin(rad);

				nodes[i].position = Vec2i(x, y);
			}
		}
	}
}
