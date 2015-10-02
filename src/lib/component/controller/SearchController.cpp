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
	if (!message->isEdge)
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
	LOG_INFO("autocomplete string: \"" + message->word + "\"");
	getView()->setAutocompletionList(m_storageAccess->getAutocompletionMatches(message->query, message->word));
}

SearchView* SearchController::getView()
{
	return Controller::getView<SearchView>();
}
