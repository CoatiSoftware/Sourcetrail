#include "component/controller/RefreshController.h"

#include "component/view/RefreshView.h"

RefreshController::RefreshController()
	: m_autoRefreshEnabled(false)
{
}

RefreshController::~RefreshController()
{
}

void RefreshController::handleMessage(MessageAutoRefreshChanged* message)
{
	m_autoRefreshEnabled = message->enabled;
}

void RefreshController::handleMessage(MessageRefresh* message)
{
	getView()->refreshView();
}

void RefreshController::handleMessage(MessageWindowFocus* message)
{
	if (m_autoRefreshEnabled)
	{
		MessageRefresh().dispatch();
	}
}

RefreshView* RefreshController::getView()
{
	return Controller::getView<RefreshView>();
}
