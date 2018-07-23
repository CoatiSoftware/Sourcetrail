#include "component/controller/SearchController.h"

#include "component/view/SearchView.h"
#include "data/access/StorageAccess.h"
#include "utility/logging/logging.h"
#include "utility/tracing.h"

SearchController::SearchController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
{
}

SearchController::~SearchController()
{
}

void SearchController::handleMessage(MessageActivateAll* message)
{
	getView()->setMatches(message->getSearchMatches());
}

void SearchController::handleMessage(MessageActivateErrors* message)
{
	getView()->setMatches(message->getSearchMatches());
}

void SearchController::handleMessage(MessageActivateFullTextSearch* message)
{
	getView()->setMatches(message->getSearchMatches());
}

void SearchController::handleMessage(MessageActivateLegend* message)
{
	getView()->setMatches(message->getSearchMatches());
}

void SearchController::handleMessage(MessageActivateTokens* message)
{
	if (message->keepContent())
	{
		return;
	}

	getView()->setMatches(message->getSearchMatches());
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
	getView()->setMatches(std::vector<SearchMatch>());
}
