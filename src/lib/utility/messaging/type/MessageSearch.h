#ifndef MESSAGE_SEARCH_H
#define MESSAGE_SEARCH_H

#include "utility/messaging/Message.h"

#include "data/NodeTypeSet.h"
#include "data/search/SearchMatch.h"

class MessageSearch
	: public Message<MessageSearch>
{
public:
	static const std::string getStaticType()
	{
		return "MessageSearch";
	}

	MessageSearch(const std::vector<SearchMatch>& matches, NodeTypeSet acceptedNodeTypes = NodeTypeSet::all())
		: isFromSearch(true)
		, acceptedNodeTypes(acceptedNodeTypes)
		, m_matches(matches)
	{
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

	std::vector<NameHierarchy> getTokenNamesOfMatches() const
	{
		std::vector<NameHierarchy> tokenNames;
		for (const SearchMatch& match : m_matches)
		{
			tokenNames.push_back(match.tokenName);
		}
		return tokenNames;
	}

	virtual void print(std::wostream& os) const
	{
		for (const SearchMatch& match : m_matches)
		{
			os << " @" << match.name << "-" << match.tokenName.getQualifiedName();
		}
	}

	bool isFromSearch;
	NodeTypeSet acceptedNodeTypes;

private:
	const std::vector<SearchMatch> m_matches;
};

#endif // MESSAGE_SEARCH_H
