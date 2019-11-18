#include "TooltipView.h"

TooltipView::TooltipView(ViewLayout* viewLayout): View(viewLayout) {}

TooltipView::~TooltipView() {}

std::string TooltipView::getName() const
{
	return "TooltipView";
}
