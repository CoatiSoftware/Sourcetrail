#include "component/controller/GraphLayouter.h"

#include "component/view/graphElements/GraphNode.h"

void GraphLayouter::layoutSimpleRaster(std::vector<DummyNode>& nodes)
{
	int x = 0;
	int y = 0;
	int offset = 150;

	for(unsigned int i = 0; i < nodes.size(); i++)
	{
		nodes[i].position = Vec2i(x, y);

		if(x > 0)
		{
			y += offset;
			x = 0;
		}
		else
		{
			x += offset;
		}
	}
}

void GraphLayouter::layoutSimpleRing(std::vector<DummyNode>& nodes)
{
	if(nodes.size() >= 1)
	{
		nodes[0].position = Vec2i(0, 0);

		if(nodes.size() > 1)
		{
			int offset = 150;

			for(unsigned int i = 1; i < nodes.size(); i++)
			{
				float rad = ((2.0f*3.14159265359f) / float(nodes.size()-1)) * i-1;

				int x = (int)((float)offset * std::cos(rad));
				int y = (int)((float)offset * std::sin(rad));

				nodes[i].position = Vec2i(x, y);
			}
		}
	}
}
