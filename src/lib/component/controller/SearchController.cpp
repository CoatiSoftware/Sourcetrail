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

void SearchController::handleMessage(MessageActivateAll* message)
{
	SearchMatch match = SearchMatch::createCommand(SearchMatch::COMMAND_ALL);
	getView()->setMatches(std::vector<SearchMatch>(1, match));
}

void SearchController::handleMessage(MessageActivateTokens* message)
{
	if (!message->isFromSearch)
	{
		if (!message->keepContent() && message->tokenIds.size())
		{
			getView()->setMatches(m_storageAccess->getSearchMatchesForTokenIds(message->tokenIds));
		}
		else if ((message->isReplayed() || message->unknownNames.size()) && !message->tokenIds.size())
		{
			std::vector<SearchMatch> matches;

			for (const std::string& name : message->unknownNames)
			{
				matches.push_back(SearchMatch(name));
			}

			if (!matches.size())
			{
				matches.push_back(SearchMatch("<invalid>"));
			}

			getView()->setMatches(matches);
		}
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

void SearchController::handleMessage(MessageSearchFullText* message)
{
	LOG_INFO("fulltext string: \"" + message->searchTerm + "\"");
	std::string prefix = "@";
	if (message->caseSensitive)
	{
		prefix += "@";
	}
	SearchMatch match(prefix + message->searchTerm);
	match.searchType = SearchMatch::SEARCH_FULLTEXT;
	getView()->setMatches(std::vector<SearchMatch>(1, match));
}

void SearchController::handleMessage(MessageShowErrors* message)
{
	SearchMatch match = SearchMatch::createCommand(SearchMatch::COMMAND_ERROR);
	getView()->setMatches(std::vector<SearchMatch>(1, match));
}

SearchView* SearchController::getView()
{
	return Controller::getView<SearchView>();
}
