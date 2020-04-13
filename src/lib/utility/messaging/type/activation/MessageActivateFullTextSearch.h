#ifndef MESSAGE_ACTIVATE_FULLTEXT_SEARCH_H
#define MESSAGE_ACTIVATE_FULLTEXT_SEARCH_H

#include "../../Message.h"
#include "MessageActivateBase.h"
#include "../../../../component/TabId.h"

class MessageActivateFullTextSearch
	: public Message<MessageActivateFullTextSearch>
	, public MessageActivateBase
{
public:
	static const std::string getStaticType()
	{
		return "MessageActivateFullTextSearch";
	}

	MessageActivateFullTextSearch(const std::wstring& searchTerm, bool caseSensitive = false)
		: searchTerm(searchTerm), caseSensitive(caseSensitive)
	{
		setSchedulerId(TabId::currentTab());
	}

	void print(std::wostream& os) const override
	{
		os << searchTerm;
	}

	std::vector<SearchMatch> getSearchMatches() const override
	{
		std::wstring prefix(caseSensitive ? 2 : 1, SearchMatch::FULLTEXT_SEARCH_CHARACTER);
		SearchMatch match(prefix + searchTerm);
		match.searchType = SearchMatch::SEARCH_FULLTEXT;
		return {match};
	}

	const std::wstring searchTerm;
	bool caseSensitive;
};

#endif	  // MESSAGE_ACTIVATE_FULLTEXT_SEARCH_H
