#ifndef LIST_LAYOUTER_H
#define LIST_LAYOUTER_H

#include "utility/math/Vector2.h"

struct DummyNode;

class ListLayouter
{
public:
	ListLayouter(Vec2i viewSize);

	void layoutList(std::vector<std::shared_ptr<DummyNode>>& nodes);

private:
	Vec2i m_viewSize;
};

#endif // LIST_LAYOUTER_H
