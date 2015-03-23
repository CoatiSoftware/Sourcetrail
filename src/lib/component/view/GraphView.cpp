#include "component/view/GraphView.h"

GraphView::GraphView(ViewLayout* viewLayout)
	: View(viewLayout)
{
}

GraphView::~GraphView()
{
}

std::string GraphView::getName() const
{
	return "GraphView";
}
