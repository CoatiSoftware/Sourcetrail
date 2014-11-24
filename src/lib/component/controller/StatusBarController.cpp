#include "component/controller/StatusBarController.h"

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

void StatusBarController::handleMessage(MessageFinishedParsing* message)
{
	setStatus("Parsing Finished");
}

void StatusBarController::handleMessage(MessageStatus* message)
{
	setStatus(message->status);
}

void StatusBarController::handleMessage(MessageError* message)
{
    setStatus(message->error, true);
}

void StatusBarController::setStatus(const std::string& status, bool isError)
{
    if(!status.empty())
        getView()->showMessage(status, isError);
}

