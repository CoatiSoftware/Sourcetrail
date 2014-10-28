#include "component/controller/SearchController.h"

#include "component/view/SearchView.h"
#include "data/access/GraphAccess.h"
#include "utility/messaging/type/MessageActivateTokens.h"

SearchController::SearchController(GraphAccess* graphAccess)
	: m_graphAccess(graphAccess)
	, m_ignoreNextMessageActivateToken(false)
{
}

SearchController::~SearchController()
{
}

void SearchController::handleMessage(MessageActivateToken* message)
{
	if (!m_ignoreNextMessageActivateToken && message->tokenId)
	{
		std::string name = m_graphAccess->getNameForNodeWithId(message->tokenId);
		std::stringstream ss;
		ss << '"' << name << ',' << message->tokenId << '"';
		getView()->setText(ss.str());
	}

	m_ignoreNextMessageActivateToken = false;
}

void SearchController::handleMessage(MessageFind* message)
{
	getView()->setFocus();
}

void SearchController::handleMessage(MessageRefresh* message)
{
	getView()->refreshView();
}

void SearchController::handleMessage(MessageSearch* message)
{
	const std::string& query = message->query;

	LOG_INFO("search string: \"" + query + "\"");

	m_ignoreNextMessageActivateToken = true;

	std::vector<Id> ids = m_graphAccess->getTokenIdsForQuery(query);
	if (ids.size())
	{
		MessageActivateTokens(ids).dispatch();
		return;
	}

	Id nodeId = m_graphAccess->getIdForNodeWithName(query);
	MessageActivateToken(nodeId).dispatch();
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
