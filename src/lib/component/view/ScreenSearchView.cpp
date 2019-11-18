#include "ScreenSearchView.h"

ScreenSearchView::ScreenSearchView(ViewLayout* viewLayout): View(viewLayout) {}

ScreenSearchView::~ScreenSearchView() {}

std::string ScreenSearchView::getName() const
{
	return "ScreenSearchView";
}
