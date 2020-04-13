#include "RefreshView.h"

#include "../controller/RefreshController.h"

RefreshView::RefreshView(ViewLayout* viewLayout): View(viewLayout) {}

RefreshView::~RefreshView() {}

std::string RefreshView::getName() const
{
	return "RefreshView";
}

RefreshController* RefreshView::getController()
{
	return View::getController<RefreshController>();
}
