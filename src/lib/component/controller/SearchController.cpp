#include "SearchController.h"

#include "logging.h"
#include "MessageTabState.h"
#include "SearchView.h"
#include "StorageAccess.h"
#include "tracing.h"

SearchController::SearchController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
{
}

Id SearchController::getSchedulerId() const
{
	return Controller::getTabId();
}

void SearchController::handleMessage(MessageActivateAll* message)
{
	updateMatches(message);
}

void SearchController::handleMessage(MessageActivateErrors* message)
{
	updateMatches(message);
}

void SearchController::handleMessage(MessageActivateFullTextSearch* message)
{
	updateMatches(message);
}

void SearchController::handleMessage(MessageActivateLegend* message)
{
	updateMatches(message);
}

void SearchController::handleMessage(MessageActivateTokens* message)
{
	if (!message->isEdge)
	{
		updateMatches(message, !message->keepContent());
	}
}

void SearchController::handleMessage(MessageFind* message)
{
	if (message->findFulltext)
	{
		getView()->findFulltext();
	}
	else
	{
		getView()->setFocus();
	}
}

void SearchController::handleMessage(MessageSearchAutocomplete* message)
{
	TRACE("search autocomplete");

	SearchView* view = getView();

	// Don't autocomplete if autocompletion request is not up-to-date anymore
	if (message->query != view->getQuery())
	{
		return;
	}

	LOG_INFO(L"autocomplete string: \"" + message->query + L"\"");
	view->setAutocompletionList(m_storageAccess->getAutocompletionMatches(message->query, message->acceptedNodeTypes));
}

SearchView* SearchController::getView()
{
	return Controller::getView<SearchView>();
}

void SearchController::clear()
{
	updateMatches(nullptr);
}

void SearchController::updateMatches(MessageActivateBase* message, bool updateView)
{
	std::vector<SearchMatch> matches;

	if (message)
	{
		matches = message->getSearchMatches();
	}

	if (updateView)
	{
		getView()->setMatches(matches);
	}

	MessageTabState(Controller::getTabId(), matches).dispatch();
}
