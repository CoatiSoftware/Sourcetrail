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

	getView()->setMatches(std::deque<SearchMatch>());
}

void SearchController::handleMessage(MessageActivateFile* message)
{
	SearchMatch match;
	match.fullName = message->filePath.fileName();
	match.nodeType = Node::NODE_FILE;
	match.tokenIds.insert(m_storageAccess->getTokenIdForFileNode(message->filePath));
	match.queryNodeType = QueryNode::QUERYNODETYPE_TOKEN;

	getView()->setMatches(std::deque<SearchMatch>(1, match));
}

void SearchController::handleMessage(MessageActivateNode* message)
{
	SearchMatch match;
	match.fullName = message->name;
	match.nodeType = message->type;
	match.tokenIds.insert(message->tokenId);
	match.queryNodeType = QueryNode::QUERYNODETYPE_TOKEN;

	getView()->setMatches(std::deque<SearchMatch>(1, match));
}

void SearchController::handleMessage(MessageFind* message)
{
	getView()->setFocus();
}

void SearchController::handleMessage(MessageFinishedParsing* message)
{
	getView()->setMatches(std::deque<SearchMatch>());
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
