#include "component/controller/ErrorController.h"

#include "data/access/StorageAccess.h"

ErrorController::ErrorController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
{
}

ErrorController::~ErrorController()
{
}

void ErrorController::handleMessage(MessageClearErrorCount* message)
{
	clear();
}

void ErrorController::handleMessage(MessageFinishedParsing* message)
{
	clear();

	getView()->addErrors(m_storageAccess->getErrors(), false);
}

void ErrorController::handleMessage(MessageNewErrors* message)
{
	getView()->addErrors(message->errors, true);
	getView()->showDockWidget();
}

void ErrorController::handleMessage(MessageShowErrors* message)
{
	if (message->errorId)
	{
		getView()->setErrorId(message->errorId);
		return;
	}

	clear();

	std::vector<ErrorInfo> errors = m_storageAccess->getErrors();
	if (errors.size())
	{
		getView()->showDockWidget();
	}
	getView()->addErrors(errors, false);
}

ErrorView* ErrorController::getView() const
{
	return Controller::getView<ErrorView>();
}

void ErrorController::clear()
{
	getView()->clear();
}
