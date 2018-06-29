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

void ErrorController::errorFilterChanged(const ErrorFilter& filter)
{
	if (m_activeFilePath.empty())
	{
		MessageActivateErrors(filter).dispatch();
	}
	else
	{
		MessageActivateErrors(filter, m_activeFilePath).dispatch();
	}
}

void ErrorController::showError(Id errorId)
{
	if (!m_showsErrors)
	{
		errorFilterChanged(getView()->getErrorFilter());
	}

	MessageShowError(errorId).dispatch();
}

void ErrorController::handleMessage(MessageActivateAll* message)
{
	m_showsErrors = false;
}

void ErrorController::handleMessage(MessageActivateErrors* message)
{
	clear();

	m_showsErrors = true;
	m_activeFilePath = message->file;

	ErrorView* view = getView();
	view->setErrorFilter(message->filter);

	if (showErrors(message->filter, true))
	{
		view->showDockWidget();
	}
}

void ErrorController::handleMessage(MessageActivateTokens* message)
{
	m_showsErrors = false;
}

void ErrorController::handleMessage(MessageErrorCountClear* message)
{
	clear();

	ErrorView* view = getView();

	ErrorFilter filter = view->getErrorFilter();
	filter.limit = ErrorFilter().limit;
	view->setErrorFilter(filter);
}

void ErrorController::handleMessage(MessageErrorCountUpdate* message)
{
	ErrorFilter filter = getView()->getErrorFilter();

	int room = filter.limit - m_errorCount;
	if (room > 0)
	{
		filter.limit = 0;
		std::vector<ErrorInfo> errors = m_storageAccess->getErrorsLimited(filter);
		ErrorCountInfo errorCount(errors);

		auto startIt = errors.begin() + m_errorCount;
		errors = std::vector<ErrorInfo>(
			startIt,
			(errors.size() < m_errorCount + room) ? errors.end() : startIt + room
		);

		getView()->addErrors(errors, errorCount, true);
		getView()->showDockWidget();

		m_errorCount += errors.size();
	}
}

void ErrorController::handleMessage(MessageErrorsAll* message)
{
	MessageActivateErrors(getView()->getErrorFilter()).dispatch();
}

void ErrorController::handleMessage(MessageErrorsForFile* message)
{
	MessageActivateErrors(ErrorFilter(), message->file).dispatch();
}

void ErrorController::handleMessage(MessageErrorsHelpMessage* message)
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
	if (!message->force)
	{
		if (appSettings->getSeenErrorHelpMessage())
		{
			return;
		}

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

void ErrorController::handleMessage(MessageFinishedParsing* message)
{
	clear();

	showErrors(getView()->getErrorFilter(), false);
}

void ErrorController::handleMessage(MessageShowError* message)
{
	getView()->setErrorId(message->errorId);
}

void ErrorController::handleMessage(MessageSearchFullText* message)
{
	m_showsErrors = false;
}

ErrorView* ErrorController::getView() const
{
	return Controller::getView<ErrorView>();
}

void ErrorController::clear()
{
	m_showsErrors = false;
	m_errorCount = 0;
	m_activeFilePath = FilePath();

	getView()->clear();
}

bool ErrorController::showErrors(const ErrorFilter& filter, bool scrollTo)
{
	ErrorView* view = getView();

	ErrorFilter filterUnlimited = filter;
	filterUnlimited.limit = 0;

	std::vector<ErrorInfo> errors;
	if (m_activeFilePath.empty())
	{
		errors = m_storageAccess->getErrorsLimited(filterUnlimited);
	}
	else
	{
		errors = m_storageAccess->getErrorsForFileLimited(filter, m_activeFilePath);
	}

	ErrorCountInfo errorCount(errors);
	if (filter.limit > 0 && errors.size() > filter.limit)
	{
		errors.resize(filter.limit);
	}

	view->addErrors(errors, errorCount, scrollTo);

	return errors.size();
}
