#ifndef MESSAGE_SEARCH_H
#define MESSAGE_SEARCH_H

#include <deque>

#include "utility/messaging/Message.h"
#include "utility/utilityString.h"

#include "data/search/SearchMatch.h"

class MessageSearch
	: public Message<MessageSearch>
{
public:
	MessageSearch(const std::string& query)
		: m_query(query)
	{
	}

	MessageSearch(const std::deque<SearchMatch>& matches)
		: m_matches(matches)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageSearch";
	}

	std::string getQuery() const
	{
		if (m_query.size())
		{
			return m_query;
		}

		return utility::join(SearchMatch::searchMatchDequeToStringDeque(m_matches), "");
	}

	const std::deque<SearchMatch>& getMatches() const
	{
		return m_matches;
	}

private:
	const std::string m_query;
	const std::deque<SearchMatch> m_matches;
};

#endif // MESSAGE_SEARCH_H
