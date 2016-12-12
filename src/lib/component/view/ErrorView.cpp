#include "component/view/ErrorView.h"

ErrorView::ErrorView(ViewLayout* viewLayout)
	: View(viewLayout)
{
}

ErrorView::~ErrorView()
{
}

std::string ErrorView::getName() const
{
	return "Errors";
}
