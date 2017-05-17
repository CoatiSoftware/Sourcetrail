#ifndef MESSAGE_SEARCH_H
#define MESSAGE_SEARCH_H

#include "utility/messaging/Message.h"

#include "data/search/SearchMatch.h"

class MessageSearch
	: public Message<MessageSearch>
{
public:
	MessageSearch(const std::vector<SearchMatch>& matches)
		: isFromSearch(true)
		, m_matches(matches)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageSearch";
	}

	std::string getMatchesAsString() const
	{
		std::stringstream ss;

		for (size_t i = 0; i < m_matches.size(); i++)
		{
			ss << '@';
			if (m_matches[i].nodeType == Node::NODE_FILE)
			{
				ss << m_matches[i].subtext;
			}
			else
			{
				if (!m_matches[i].subtext.empty())
				{
					ss << m_matches[i].subtext << m_matches[i].delimiter;
				}
				ss << m_matches[i].name;
			}
		}

		return ss.str();
	}

	const std::vector<SearchMatch>& getMatches() const
	{
		return m_matches;
	}

	std::vector<Id> getTokenIdsOfMatches() const
	{
		std::vector<Id> tokenIds;
		for (const SearchMatch& match : m_matches)
		{
			for (const Id tokenId : match.tokenIds)
			{
				if (tokenId)
				{
					tokenIds.push_back(tokenId);
				}
			}
		}
		return tokenIds;
	}

	virtual void print(std::ostream& os) const
	{
		os << getMatchesAsString();
	}

	bool isFromSearch;

private:
	const std::vector<SearchMatch> m_matches;
};

#endif // MESSAGE_SEARCH_H
