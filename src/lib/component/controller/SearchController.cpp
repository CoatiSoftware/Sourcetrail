#include "component/controller/SearchController.h"

#include "component/view/SearchView.h"
#include "data/access/StorageAccess.h"

SearchController::SearchController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
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
		match.fullName = m_storageAccess->getNameForNodeWithId(message->tokenIds[0]);
		match.nodeType = m_storageAccess->getNodeTypeForNodeWithId(message->tokenIds[0]);
		match.tokenIds.insert(message->tokenIds[0]);
		match.queryNodeType = QueryNode::QUERYNODETYPE_TOKEN;


		getView()->setMatch(match);
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

	std::vector<Id> ids = m_storageAccess->getTokenIdsForQuery(query);
	if (!ids.size())
	{
		ids.push_back(m_storageAccess->getIdForNodeWithName(query));
	}

	MessageActivateTokens(ids).dispatch();
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
