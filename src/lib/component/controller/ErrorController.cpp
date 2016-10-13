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

	auto errors = m_storageAccess->getAllErrors();

	for (const StorageError& error : errors)
	{
		getView()->addError(error);
	}
}

void ErrorController::handleMessage(MessageNewErrors* message)
{
	for (const StorageError& error : message->errors)
	{
		getView()->addError(error);
	}

	getView()->showDockWidget();
}

void ErrorController::handleMessage(MessageShowErrors* message)
{
	if (message->errorId)
	{
		if (message->isReplayed())
		{
			getView()->setErrorId(message->errorId);
		}
		return;
	}

	clear();

	auto errors = m_storageAccess->getAllErrors();

	for (const StorageError& error : errors)
	{
		getView()->addError(error);
	}

	getView()->showDockWidget();
}

ErrorView* ErrorController::getView() const
{
	return Controller::getView<ErrorView>();
}

void ErrorController::clear()
{
	getView()->clear();
}
