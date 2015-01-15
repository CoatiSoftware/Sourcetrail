#include "component/controller/SearchController.h"

#include "component/view/SearchView.h"
#include "data/access/GraphAccess.h"

SearchController::SearchController(GraphAccess* graphAccess)
	: m_graphAccess(graphAccess)
	, m_ignoreNextMessageActivateTokens(false)
{
}

SearchController::~SearchController()
{
}

void SearchController::handleMessage(MessageActivateTokens* message)
{
	if (!m_ignoreNextMessageActivateTokens && message->tokenIds.size())
	{
		SearchMatch match;
		match.fullName = m_graphAccess->getNameForNodeWithId(message->tokenIds[0]);
		match.tokenIds.insert(message->tokenIds[0]);

		getView()->setText(match.encodeForQuery());
	}

	m_ignoreNextMessageActivateTokens = false;
}

void SearchController::handleMessage(MessageFind* message)
{
	getView()->setFocus();
}

void SearchController::handleMessage(MessageFinishedParsing* message)
{
	getView()->setText("");
}

void SearchController::handleMessage(MessageRefresh* message)
{
	getView()->refreshView();
}

void SearchController::handleMessage(MessageSearch* message)
{
	const std::string& query = message->query;

	LOG_INFO("search string: \"" + query + "\"");

	m_ignoreNextMessageActivateTokens = true;

	std::vector<Id> ids = m_graphAccess->getTokenIdsForQuery(query);
	if (!ids.size())
	{
		ids.push_back(m_graphAccess->getIdForNodeWithName(query));
	}

	MessageActivateTokens(ids).dispatch();
}

void SearchController::handleMessage(MessageSearchAutocomplete* message)
{
	LOG_INFO("autocomplete string: \"" + message->word + "\"");
	getView()->setAutocompletionList(m_graphAccess->getAutocompletionMatches(message->query, message->word));
}

SearchView* SearchController::getView()
{
	return Controller::getView<SearchView>();
}
