#ifndef GRAPH_POSTPROCESSOR_H
#define GRAPH_POSTPROCESSOR_H

#include <memory>
#include <list>

#include "utility/math/MatrixDynamicBase.h"

#include "component/controller/helper/DummyNode.h"

class GraphPostprocessor
{
public:
	static void doPostprocessing(std::vector<DummyNode>& nodes);

	static void alignNodeOnRaster(DummyNode& node);
	static Vec2i alignOnRaster(Vec2i position);

private:
	static unsigned int s_cellSize;
	static unsigned int s_cellPadding;

	static MatrixDynamicBase<unsigned int> buildHeatMap(const std::vector<DummyNode>& nodes, const int atomarNodeSize, const int maxNodeSize);
	static void resolveOutliers(std::vector<DummyNode>& nodes, const Vec2i& centerPoint);
	static void resolveOverlap(std::vector<DummyNode>& nodes, MatrixDynamicBase<unsigned int>& heatMap, const int divisor);
	static void modifyHeatmapArea(MatrixDynamicBase<unsigned int>& heatMap, const Vec2i& leftUpperCorner, const Vec2i& size, const int modifier);
	static bool getHeatmapGradient(Vec2f& outGradient, const MatrixDynamicBase<unsigned int>& heatMap, const Vec2i& leftUpperCorner, const Vec2i& size);
	static Vec2f heatMapRayCast(const MatrixDynamicBase<unsigned int>& heatMap, const Vec2f& startPosition, const Vec2f& direction, unsigned int minValue);
	static Vec2i calculateRasterNodeSize(const DummyNode& node);
};

#endif // GRAPH_POSTPROCESSOR_H
