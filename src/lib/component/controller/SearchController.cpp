#include "component/controller/SearchController.h"

#include "component/view/SearchView.h"
#include "data/access/StorageAccess.h"
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
	getView()->setMatches(std::vector<SearchMatch>(1, SearchMatch::createCommand(SearchMatch::COMMAND_ALL)));
}

void SearchController::handleMessage(MessageActivateTokens* message)
{
	if ((message->isFromSearch && !message->isReplayed()) || message->keepContent())
	{
		return;
	}

	if (message->searchMatches.size())
	{
		getView()->setMatches(message->searchMatches);
	}
	else if (message->tokenIds.size())
	{
		getView()->setMatches(m_storageAccess->getSearchMatchesForTokenIds(message->tokenIds));
	}
	else if (message->tokenNames.size())
	{
		std::vector<SearchMatch> matches;
		getView()->setMatches(matches);

		for (const NameHierarchy& name : message->tokenNames)
		{
			matches.push_back(SearchMatch(name.getQualifiedName()));
		}

		if (!matches.size())
		{
			matches.push_back(SearchMatch("<invalid>"));
		}

		getView()->setMatches(matches);
	}
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

	LOG_INFO("autocomplete string: \"" + message->query + "\"");
	view->setAutocompletionList(m_storageAccess->getAutocompletionMatches(message->query));
}

void SearchController::handleMessage(MessageSearchFullText* message)
{
	LOG_INFO("fulltext string: \"" + message->searchTerm + "\"");
	std::string prefix(message->caseSensitive ? 2 : 1, SearchMatch::FULLTEXT_SEARCH_CHARACTER);

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

void SearchController::clear()
{
	getView()->setMatches(std::vector<SearchMatch>());
}
