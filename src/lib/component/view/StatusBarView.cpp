#include "component/view/StatusBarView.h"

#include "component/controller/StatusBarController.h"

StatusBarView::StatusBarView(ViewLayout* viewLayout)
	: View(viewLayout, Vec2i(100,100))
{
}

StatusBarView::~StatusBarView()
{
}

std::string StatusBarView::getName() const
{
	return "StatusBarView";
}

StatusBarController* StatusBarView::getController()
{
	return View::getController<StatusBarController>();
}
