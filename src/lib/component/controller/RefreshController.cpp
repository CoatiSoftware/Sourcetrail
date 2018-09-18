#include "RefreshController.h"

#include "RefreshView.h"

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
