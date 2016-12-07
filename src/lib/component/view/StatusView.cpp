#include "component/view/StatusView.h"

StatusView::StatusView(ViewLayout* viewLayout)
	: View(viewLayout)
{
}

StatusView::~StatusView()
{
}

std::string StatusView::getName() const
{
	return "Status";
}

