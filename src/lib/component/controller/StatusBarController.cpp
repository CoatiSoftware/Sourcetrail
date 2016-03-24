#include "component/controller/StatusBarController.h"

#include "component/view/StatusBarView.h"
#include "data/access/StorageAccess.h"
#include "utility/logging/logging.h"

StatusBarController::StatusBarController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
{
}

StatusBarController::~StatusBarController()
{
}

StatusBarView* StatusBarController::getView()
{
	return Controller::getView<StatusBarView>();
}

void StatusBarController::handleMessage(MessageClearErrorCount* message)
{
	getView()->setErrorCount(ErrorCountInfo());
}

void StatusBarController::handleMessage(MessageFinishedParsing* message)
{
	ErrorCountInfo errorCount = m_storageAccess->getErrorCount();
	getView()->setErrorCount(errorCount);

	std::string status = message->getStatusStr();
	status += " " + std::to_string(errorCount.total) + " error" + (errorCount.total != 1 ? "s" : "");
	if (errorCount.fatal > 0)
	{
		status += " (" + std::to_string(errorCount.fatal) + " fatal)";
	}

	MessageStatus(status, errorCount.total > 0).dispatch();
}

void StatusBarController::handleMessage(MessageShowErrors* message)
{
	if (message->errorCount.total >= 0)
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
