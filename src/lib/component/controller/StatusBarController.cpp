#include "component/controller/StatusBarController.h"

#include "utility/logging/logging.h"

#include "component/view/StatusBarView.h"

StatusBarController::StatusBarController()
{
}

StatusBarController::~StatusBarController()
{
}

StatusBarView* StatusBarController::getView()
{
	return Controller::getView<StatusBarView>();
}

void StatusBarController::handleMessage(MessageShowErrors* message)
{
	if (message->errorCount >= 0)
	{
		getView()->setErrorCount(message->errorCount);
	}
}

void StatusBarController::handleMessage(MessageStatus* message)
{
	setStatus(message->status, message->isError, message->showLoader);
}

void StatusBarController::setStatus(const std::string& status, bool isError, bool showLoader)
{
	if (!status.empty())
	{
		LOG_INFO_STREAM(<< "STATUS " << status);

		getView()->showMessage(status, isError, showLoader);
	}
}
