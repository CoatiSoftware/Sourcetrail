#include "component/view/GraphView.h"

GraphView::GraphView(ViewLayout* viewLayout)
	: View(viewLayout, Vec2i(100, 100))
{
}

GraphView::~GraphView()
{
}

std::string GraphView::getName() const
{
	return "GraphView";
}
