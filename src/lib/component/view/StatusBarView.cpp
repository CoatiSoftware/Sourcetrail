#include "StatusBarView.h"

#include "StatusBarController.h"

StatusBarView::StatusBarView(ViewLayout* viewLayout): View(viewLayout) {}

std::string StatusBarView::getName() const
{
	return "StatusBarView";
}

StatusBarController* StatusBarView::getController()
{
	return View::getController<StatusBarController>();
}
