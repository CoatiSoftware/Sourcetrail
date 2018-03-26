#ifndef LIST_LAYOUTER_H
#define LIST_LAYOUTER_H

#include <memory>
#include <vector>

#include "utility/math/Vector2.h"
#include "utility/math/Vector4.h"

struct DummyNode;

class ListLayouter
{
public:
	static Vec2i layoutRow(std::vector<std::shared_ptr<DummyNode>>* nodes, int top, int left, int gap);
	static Vec2i layoutColumn(std::vector<std::shared_ptr<DummyNode>>* nodes, int top, int left, int gap);

	static Vec2i layoutMultiColumn(Vec2i viewSize, std::vector<std::shared_ptr<DummyNode>>* nodes);
	static Vec2i layoutSkewed(
		std::vector<std::shared_ptr<DummyNode>>* nodes, int top, int left, int gapX, int gapY, int maxWidth);

	static Vec4i boundingRect(const std::vector<std::shared_ptr<DummyNode>>& nodes);
private:
	static Vec2i layoutSimple(
		std::vector<std::shared_ptr<DummyNode>>* nodes, int top, int left, int gapX, int gapY, bool horizontal);
};

#endif // LIST_LAYOUTER_H
