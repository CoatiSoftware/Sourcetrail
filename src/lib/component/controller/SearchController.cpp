#include "component/controller/SearchController.h"

#include "component/view/SearchView.h"
#include "data/access/StorageAccess.h"

SearchController::SearchController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
{
}

SearchController::~SearchController()
{
}

void SearchController::handleMessage(MessageActivateTokens* message)
{
	if (!message->keepContent())
	{
		getView()->setMatches(m_storageAccess->getSearchMatchesForTokenIds(message->tokenIds));
	}
}

void SearchController::handleMessage(MessageFind* message)
{
	getView()->setFocus();
}

void SearchController::handleMessage(MessageSearchAutocomplete* message)
{
	LOG_INFO("autocomplete string: \"" + message->query + "\"");
	getView()->setAutocompletionList(m_storageAccess->getAutocompletionMatches(message->query));
}

SearchView* SearchController::getView()
{
	return Controller::getView<SearchView>();
}
