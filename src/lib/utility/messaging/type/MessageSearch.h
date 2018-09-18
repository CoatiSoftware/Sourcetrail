#ifndef MESSAGE_SEARCH_H
#define MESSAGE_SEARCH_H

#include "Message.h"

#include "NodeTypeSet.h"
#include "SearchMatch.h"

class MessageSearch
	: public Message<MessageSearch>
{
public:
	static const std::string getStaticType()
	{
		return "MessageSearch";
	}

	MessageSearch(const std::vector<SearchMatch>& matches, NodeTypeSet acceptedNodeTypes)
		: acceptedNodeTypes(acceptedNodeTypes)
		, m_matches(matches)
	{
	}

	const std::vector<SearchMatch>& getMatches() const
	{
		return m_matches;
	}

	virtual void print(std::wostream& os) const
	{
		for (const SearchMatch& match : m_matches)
		{
			os << " @" << match.name;
		}
	}

	NodeTypeSet acceptedNodeTypes;

private:
	const std::vector<SearchMatch> m_matches;
};

#endif // MESSAGE_SEARCH_H
