#ifndef MESSAGE_SEARCH_H
#define MESSAGE_SEARCH_H

#include "utility/messaging/Message.h"
#include "utility/utilityString.h"

#include "data/search/SearchMatch.h"

class MessageSearch
	: public Message<MessageSearch>
{
public:
	MessageSearch(const std::vector<SearchMatch>& matches)
		: m_matches(matches)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageSearch";
	}

	std::string getMatchesAsString() const
	{
		return SearchMatch::searchMatchesToString(m_matches);
	}

	const std::vector<SearchMatch>& getMatches() const
	{
		return m_matches;
	}

	virtual void print(std::ostream& os) const
	{
		os << getMatchesAsString();
	}

private:
	const std::vector<SearchMatch> m_matches;
};

#endif // MESSAGE_SEARCH_H
