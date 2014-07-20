#ifndef GRAPH_LAYOUTER_H
#define GRAPH_LAYOUTER_H

#include <vector>

struct DummyNode;

typedef void (*LayoutFunction)(std::vector<DummyNode>&);

class GraphLayouter
{
public:
	static void layoutSimpleRaster(std::vector<DummyNode>& nodes);
	static void layoutSimpleRing(std::vector<DummyNode>& nodes);
};

#endif // GRAPH_LAYOUTER_H
