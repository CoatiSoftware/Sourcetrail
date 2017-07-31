#include "component/view/GraphView.h"

const char* GraphView::VIEW_NAME = "Graph";

GraphView::GraphView(ViewLayout* viewLayout)
	: View(viewLayout)
{
}

GraphView::~GraphView()
{
}

std::string GraphView::getName() const
{
	return VIEW_NAME;
}
