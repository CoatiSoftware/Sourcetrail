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

void SearchController::handleMessage(MessageActivateEdge* message)
{
	if (!message->isAggregation())
	{
		return;
	}

	getView()->setMatches(std::vector<SearchMatch>());
}

void SearchController::handleMessage(MessageActivateFile* message)
{
	SearchMatch match;
	match.fullName = message->filePath.fileName();
	match.nodeType = Node::NODE_FILE;
	match.tokenIds.insert(m_storageAccess->getTokenIdForFileNode(message->filePath));
	match.searchType = SearchMatch::SEARCH_TOKEN;

	getView()->setMatches(std::vector<SearchMatch>(1, match));
}

void SearchController::handleMessage(MessageActivateNodes* message)
{
	std::vector<SearchMatch> matches;

	for (const MessageActivateNodes::ActiveNode& node : message->nodes)
	{
		SearchMatch match;
		match.fullName = node.name;
		match.nodeType = node.type;
		match.tokenIds.insert(node.nodeId);
		match.searchType = SearchMatch::SEARCH_TOKEN;
		matches.push_back(match);
	}

	getView()->setMatches(matches);
}

void SearchController::handleMessage(MessageFind* message)
{
	getView()->setFocus();
}

void SearchController::handleMessage(MessageFinishedParsing* message)
{
	getView()->setMatches(std::vector<SearchMatch>());
}

void SearchController::handleMessage(MessageSearch* message)
{
	getView()->setMatches(message->getMatches());
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
