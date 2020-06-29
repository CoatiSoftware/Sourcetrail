#ifndef MESSAGE_ACTIVATE_TOKENS_H
#define MESSAGE_ACTIVATE_TOKENS_H

#include "Message.h"
#include "MessageActivateBase.h"
#include "types.h"

#include "SearchMatch.h"

class MessageActivateTokens
	: public Message<MessageActivateTokens>
	, public MessageActivateBase
{
public:
	static const std::string getStaticType()
	{
		return "MessageActivateTokens";
	}

	MessageActivateTokens(const MessageBase* other)
		: isEdge(false), isBundledEdges(false), isFromSearch(false)
	{
		setIsParallel(true);
		setKeepContent(other->keepContent());
		setSchedulerId(other->getSchedulerId());
	}

	void print(std::wostream& os) const override
	{
		for (const Id& id: tokenIds)
		{
			os << id << L" ";
		}

		for (const SearchMatch& match: searchMatches)
		{
			for (const NameHierarchy& name: match.tokenNames)
			{
				os << name.getQualifiedName() << L" ";
			}
		}
	}

	std::vector<SearchMatch> getSearchMatches() const override
	{
		if (isBundledEdges)
		{
			SearchMatch match;
			match.name = match.text = L"bundled edges";	 // TODO: show bundled edges source and target
			match.searchType = SearchMatch::SEARCH_TOKEN;
			match.nodeType = NodeType(NODE_TYPE);
			return {match};
		}

		return searchMatches;
	}

	std::vector<Id> tokenIds;
	std::vector<SearchMatch> searchMatches;

	bool isEdge;
	bool isBundledEdges;
	bool isFromSearch;
};

#endif	  // MESSAGE_ACTIVATE_TOKENS_H
