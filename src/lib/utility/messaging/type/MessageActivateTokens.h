#ifndef MESSAGE_ACTIVATE_TOKENS_H
#define MESSAGE_ACTIVATE_TOKENS_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

#include "data/search/SearchMatch.h"

class MessageActivateTokens
	: public Message<MessageActivateTokens>
{
public:
	MessageActivateTokens(const MessageBase* other)
		: isEdge(false)
		, isAggregation(false)
		, isFromSearch(false)
	{
		setIsParallel(true);
		setKeepContent(other->keepContent());
	}

	static const std::string getStaticType()
	{
		return "MessageActivateTokens";
	}

	virtual void print(std::ostream& os) const
	{
		for (const Id& id : tokenIds)
		{
			os << id << " ";
		}
	}

	std::vector<Id> tokenIds;
	std::vector<NameHierarchy> tokenNames;
	std::vector<SearchMatch> searchMatches;

	bool isEdge;
	bool isAggregation;
	bool isFromSearch;
};

#endif // MESSAGE_ACTIVATE_TOKENS_H
