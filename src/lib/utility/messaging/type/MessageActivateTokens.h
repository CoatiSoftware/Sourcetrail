#ifndef MESSAGE_ACTIVATE_TOKENS_H
#define MESSAGE_ACTIVATE_TOKENS_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

#include "data/search/SearchMatch.h"

class MessageActivateTokens
	: public Message<MessageActivateTokens>
{
public:
	static const std::string getStaticType()
	{
		return "MessageActivateTokens";
	}

	MessageActivateTokens(const MessageBase* other)
		: isEdge(false)
		, isAggregation(false)
		, isFromSearch(false)
	{
		setIsParallel(true);
		setKeepContent(other->keepContent());
	}

	virtual void print(std::wostream& os) const
	{
		for (const Id& id : tokenIds)
		{
			os << id << L" ";
		}
	}

	std::vector<NameHierarchy> getTokenNamesOfMatches() const
	{
		std::vector<NameHierarchy> tokenNames;
		for (const SearchMatch& match : searchMatches)
		{
			tokenNames.push_back(match.tokenName);
		}
		return tokenNames;
	}

	std::vector<Id> tokenIds;
	std::vector<SearchMatch> searchMatches;

	bool isEdge;
	bool isAggregation;
	bool isFromSearch;
};

#endif // MESSAGE_ACTIVATE_TOKENS_H
