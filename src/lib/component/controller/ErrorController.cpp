#include "component/controller/ErrorController.h"

#include "data/access/StorageAccess.h"
#include "settings/ApplicationSettings.h"

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

	getView()->resetErrorLimit();
}

void ErrorController::handleMessage(MessageFinishedParsing* message)
{
	clear();

	getView()->setErrorCount(m_storageAccess->getErrorCount());
	getView()->addErrors(m_storageAccess->getErrorsLimited(), false);
}

void ErrorController::handleMessage(MessageNewErrors* message)
{
	ErrorFilter filter;
	int room = message->errors.size() + filter.limit - message->errorCount.total;
	if (room > 0)
	{
		std::vector<ErrorInfo> errors = message->errors;
		if (room < int(errors.size()))
		{
			errors.resize(room);
		}

		getView()->addErrors(message->errors, true);
		getView()->showDockWidget();
	}

	getView()->setErrorCount(message->errorCount);

}

void ErrorController::handleMessage(MessageShowErrorHelpMessage* message)
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
	if (!message->force && appSettings->getSeenErrorHelpMessage())
	{
		return;
	}

	if (!message->force)
	{
		ErrorCountInfo info = m_storageAccess->getErrorCount();
		if (!info.total)
		{
			return;
		}
	}

	appSettings->setSeenErrorHelpMessage(true);
	appSettings->save();

	getView()->showErrorHelpMessage();
}

void ErrorController::handleMessage(MessageShowErrors* message)
{
	if (message->errorId)
	{
		getView()->setErrorId(message->errorId);
		return;
	}

	clear();

	std::vector<ErrorInfo> errors = m_storageAccess->getErrorsLimited();
	if (errors.size())
	{
		getView()->showDockWidget();
	}

	getView()->setErrorCount(message->errorCount);
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
