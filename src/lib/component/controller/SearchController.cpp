#include "component/controller/SearchController.h"

#include "component/view/SearchView.h"
#include "data/access/GraphAccess.h"
#include "utility/messaging/type/MessageActivateTokens.h"

SearchController::SearchController(GraphAccess* graphAccess)
	: m_graphAccess(graphAccess)
{
}

SearchController::~SearchController()
{
}

void SearchController::handleMessage(MessageActivateToken* message)
{
	getView()->setText(m_graphAccess->getNameForNodeWithId(message->tokenId));
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

	std::vector<Id> ids = m_graphAccess->getTokenIdsForQuery(query);
	if (ids.size())
	{
		MessageActivateTokens(ids).dispatch();
		return;
	}

	Id nodeId = m_graphAccess->getIdForNodeWithName(query);
	if (nodeId > 0)
	{
		MessageActivateToken message(nodeId);
		message.dispatch();
	}
}

void SearchController::handleMessage(MessageSearchAutocomplete* message)
{
	LOG_INFO("autocomplete string: \"" + message->query + "\"");
	getView()->setAutocompletionList(m_graphAccess->getAutocompletionMatches(message->query));
}

SearchView* SearchController::getView()
{
	return Controller::getView<SearchView>();
}
