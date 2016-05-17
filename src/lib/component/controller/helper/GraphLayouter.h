#ifndef GRAPH_LAYOUTER_H
#define GRAPH_LAYOUTER_H

#include <vector>

#include "utility/math/MatrixDynamicBase.h"
#include "utility/math/Vector2.h"
#include "utility/types.h"

struct DummyEdge;
struct DummyNode;

class GraphLayouter
{
public:
	static void layoutSimpleRaster(std::vector<DummyNode>& nodes);
	static void layoutSimpleRing(std::vector<DummyNode>& nodes);

	static void layoutSpectralPrototype(std::vector<DummyNode>& nodes, const std::vector<DummyEdge>& edges);

private:
	static MatrixDynamicBase<int> buildLaplacianMatrix(
		const std::vector<DummyNode>& nodes, const std::vector<DummyEdge>& edges);
};

#endif // GRAPH_LAYOUTER_H
