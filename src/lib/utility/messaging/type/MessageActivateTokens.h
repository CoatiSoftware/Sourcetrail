#ifndef MESSAGE_ACTIVATE_TOKENS_H
#define MESSAGE_ACTIVATE_TOKENS_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageActivateTokens
	: public Message<MessageActivateTokens>
{
public:
	MessageActivateTokens(const MessageBase* other, const std::vector<Id>& tokenIds)
		: tokenIds(tokenIds)
		, isEdge(false)
		, isAggregation(false)
		, isFromSearch(false)
	{
		setIsReplayed(other->isReplayed());
		setKeepContent(other->keepContent());
		setIsLast(other->isLast());
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

	const std::vector<Id> tokenIds;

	bool isEdge;
	bool isAggregation;
	bool isFromSearch;

	std::vector<std::string> unknownNames;
};

#endif // MESSAGE_ACTIVATE_TOKENS_H
