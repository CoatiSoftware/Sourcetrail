#include "ErrorController.h"

#include "Application.h"
#include "ApplicationSettings.h"
#include "DialogView.h"
#include "Project.h"
#include "QtHelpButtonInfo.h"
#include "StorageAccess.h"
#include "TabId.h"

ErrorController::ErrorController(StorageAccess* storageAccess): m_storageAccess(storageAccess) {}

ErrorController::~ErrorController() {}

void ErrorController::errorFilterChanged(const ErrorFilter& filter)
{
	if (m_tabActiveFilePath[TabId::currentTab()].empty())
	{
		MessageActivateErrors(filter).dispatch();
	}
	else
	{
		MessageActivateErrors(filter, m_tabActiveFilePath[TabId::currentTab()]).dispatch();
	}
}

void ErrorController::showError(Id errorId)
{
	if (!m_tabShowsErrors[TabId::currentTab()] || m_newErrorsAdded)
	{
		errorFilterChanged(getView()->getErrorFilter());
	}

	MessageShowError(errorId).dispatch();
}

void ErrorController::handleActivation(const MessageActivateBase* message)
{
	m_tabShowsErrors[dynamic_cast<const MessageBase*>(message)->getSchedulerId()] = false;
}

void ErrorController::handleMessage(MessageActivateErrors* message)
{
	clear();

	m_tabShowsErrors[message->getSchedulerId()] = true;
	m_tabActiveFilePath[message->getSchedulerId()] = message->file;

	ErrorView* view = getView();
	view->setErrorFilter(message->filter);

	if (showErrors(message->filter, true))
	{
		view->showDockWidget();
	}
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
	m_storageAccess->addErrorsToCache(message->newErrors, message->errorCount);
	m_newErrorsAdded = true;

	ErrorFilter filter = getView()->getErrorFilter();

	int room = filter.limit - m_errorCount;
	if (room > 0)
	{
		filter.limit = 0;
		std::vector<ErrorInfo> errors;

		for (const ErrorInfo& error: message->newErrors)
		{
			if (filter.filter(error))
			{
				errors.push_back(error);

				if (room > 0 && errors.size() >= size_t(room))
				{
					break;
				}
			}
		}

		getView()->addErrors(errors, message->errorCount, true);

		if (!Application::getInstance()->getDialogView(DialogView::UseCase::INDEXING)->dialogsHidden())
		{
			getView()->showDockWidget();
		}

		m_errorCount += errors.size();
	}
}

void ErrorController::handleMessage(MessageErrorsAll* message)
{
	MessageActivateErrors(getView()->getErrorFilter()).dispatch();
}

void ErrorController::handleMessage(MessageErrorsForFile* message)
{
	std::shared_ptr<const Project> project = Application::getInstance()->getCurrentProject();
	if (project && project->isIndexing())
	{
		Application::getInstance()->handleDialog(
			L"Showing errors for a file is not possible while indexing.");
		return;
	}

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

	m_onQtThread([=]() { createErrorHelpButtonInfo().displayMessage(nullptr); });
}

void ErrorController::handleMessage(MessageIndexingFinished* message)
{
	m_storageAccess->setUseErrorCache(false);

	clear();

	showErrors(getView()->getErrorFilter(), false);
}

void ErrorController::handleMessage(MessageIndexingStarted* message)
{
	m_storageAccess->setUseErrorCache(true);
}

void ErrorController::handleMessage(MessageShowError* message)
{
	getView()->setErrorId(message->errorId);
}

ErrorView* ErrorController::getView() const
{
	return Controller::getView<ErrorView>();
}

void ErrorController::clear()
{
	m_errorCount = 0;
	m_tabShowsErrors.clear();
	m_tabActiveFilePath.clear();
	m_newErrorsAdded = false;

	getView()->clear();
}

bool ErrorController::showErrors(const ErrorFilter& filter, bool scrollTo)
{
	ErrorView* view = getView();

	ErrorFilter filterUnlimited = filter;
	filterUnlimited.limit = 0;

	std::vector<ErrorInfo> errors;
	if (m_tabActiveFilePath[TabId::currentTab()].empty())
	{
		errors = m_storageAccess->getErrorsLimited(filterUnlimited);
	}
	else
	{
		errors = m_storageAccess->getErrorsForFileLimited(
			filter, m_tabActiveFilePath[TabId::currentTab()]);
	}

	ErrorCountInfo errorCount(errors);
	if (filter.limit > 0 && errors.size() > filter.limit)
	{
		errors.resize(filter.limit);
	}

	view->addErrors(errors, errorCount, scrollTo);

	return errors.size();
}
