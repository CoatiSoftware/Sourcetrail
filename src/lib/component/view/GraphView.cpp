#include "component/view/GraphView.h"

GraphView::Metrics::Metrics()
	: width(0)
	, height(0)
	, typeNameCharWidth(0.0f)
	, variableNameCharWidth(0.0f)
	, functionNameCharWidth(0.0f)
{
}

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
