#ifndef GRAPH_LAYOUTER_H
#define GRAPH_LAYOUTER_H

#include <vector>

struct DummyNode;

class GraphLayouter
{
public:
	typedef void (*LayoutFunction)(std::vector<DummyNode>&);

	static void layoutSimpleRaster(std::vector<DummyNode>& nodes);
	static void layoutSimpleRing(std::vector<DummyNode>& nodes);
};

#endif // GRAPH_LAYOUTER_H
