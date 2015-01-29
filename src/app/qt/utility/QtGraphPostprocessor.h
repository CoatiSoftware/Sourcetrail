#ifndef QT_GRAPH_POSTPROCESSOR_H
#define QT_GRAPH_POSTPROCESSOR_H

#include <memory>
#include <list>

#include "utility/math/MatrixDynamicBase.h"

#include "qt/view/graphElements/QtGraphNode.h"

class QtGraphPostprocessor
{
public:
	static void doPostprocessing(std::list<std::shared_ptr<QtGraphNode>>& nodes);

private:
	static MatrixDynamicBase<unsigned int> buildHeatMap(const std::list<std::shared_ptr<QtGraphNode>>& nodes, const int atomarNodeSize, const int maxNodeSize);
	static void resolveOutliers(std::list<std::shared_ptr<QtGraphNode>>& nodes, const Vec2i& centerPoint);
	static void resolveOverlap(std::list<std::shared_ptr<QtGraphNode>>& nodes, MatrixDynamicBase<unsigned int>& heatMap, const int divisor);
	static void modifyHeatmapArea(MatrixDynamicBase<unsigned int>& heatMap, const Vec2i& leftUpperCorner, const Vec2i& size, const int modifier);
	static bool getHeatmapGradient(Vec2f& outGradient, const MatrixDynamicBase<unsigned int>& heatMap, const Vec2i& leftUpperCorner, const Vec2i& size);
	static void resizeNodes(std::list<std::shared_ptr<QtGraphNode>>& nodes, const unsigned int atomarSize);
};

#endif // QT_GRAPH_POSTPROCESSOR_H
