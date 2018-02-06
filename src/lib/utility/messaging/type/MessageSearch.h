#ifndef MESSAGE_SEARCH_H
#define MESSAGE_SEARCH_H

#include "utility/messaging/Message.h"

#include "data/NodeTypeSet.h"
#include "data/search/SearchMatch.h"

class MessageSearch
	: public Message<MessageSearch>
{
public:
	MessageSearch(const std::vector<SearchMatch>& matches, NodeTypeSet acceptedNodeTypes = NodeTypeSet::all())
		: isFromSearch(true)
		, acceptedNodeTypes(acceptedNodeTypes)
		, m_matches(matches)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageSearch";
	}

	std::wstring getMatchesAsString() const
	{
		std::wstringstream ss;

		for (size_t i = 0; i < m_matches.size(); i++)
		{
			ss << '@';
			if (m_matches[i].nodeType.isFile())
			{
				ss << m_matches[i].subtext;
			}
			else
			{
				if (!m_matches[i].subtext.empty())
				{
					ss << m_matches[i].subtext << nameDelimiterTypeToString(m_matches[i].delimiter) << m_matches[i].text;
				}
				else
				{
					ss << m_matches[i].name;
				}
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

	virtual void print(std::wostream& os) const
	{
		os << getMatchesAsString();
	}

	bool isFromSearch;
	NodeTypeSet acceptedNodeTypes;

private:
	const std::vector<SearchMatch> m_matches;
};

#endif // MESSAGE_SEARCH_H
