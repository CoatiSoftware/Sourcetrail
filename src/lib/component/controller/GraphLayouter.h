#ifndef GRAPH_LAYOUTER_H
#define GRAPH_LAYOUTER_H

#include <vector>

template<class T>
class MatrixDynamicBase;

struct DummyEdge;
struct DummyNode;

class GraphLayouter
{
public:
	typedef void (*LayoutFunction)(std::vector<DummyNode>&);

	static void layoutSimpleRaster(std::vector<DummyNode>& nodes);
	static void layoutSimpleRing(std::vector<DummyNode>& nodes);

	static void layoutSpectralPrototype(std::vector<DummyNode>& nodes, const std::vector<DummyEdge>& edges);

private:
	static MatrixDynamicBase<int> buildLaplacianMatrix(const std::vector<DummyNode>& nodes, const std::vector<DummyEdge>& edges);
};

#endif // GRAPH_LAYOUTER_H
