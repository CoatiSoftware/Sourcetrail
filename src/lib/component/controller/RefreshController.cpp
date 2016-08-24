#include "component/controller/RefreshController.h"

#include "component/view/RefreshView.h"

RefreshController::RefreshController()
{
}

RefreshController::~RefreshController()
{
}

void RefreshController::clear()
{
}

RefreshView* RefreshController::getView()
{
	return Controller::getView<RefreshView>();
}
