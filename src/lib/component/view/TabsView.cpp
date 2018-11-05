#include "TabsView.h"

TabsView::TabsView(ViewLayout* viewLayout)
	: View(viewLayout)
{
}

std::string TabsView::getName() const
{
	return "Tabs";
}
