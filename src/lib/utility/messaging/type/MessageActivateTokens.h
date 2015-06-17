#ifndef MESSAGE_ACTIVATE_TOKENS_H
#define MESSAGE_ACTIVATE_TOKENS_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageActivateTokens: public Message<MessageActivateTokens>
{
public:
	MessageActivateTokens(const std::vector<Id>& tokenIds)
		: tokenIds(tokenIds)
		, isEdge(false)
		, isFromSystem(false)
	{
	}

	MessageActivateTokens(Id tokenId)
		: tokenIds(1, tokenId)
		, isEdge(false)
		, isFromSystem(false)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageActivateTokens";
	}

	const std::vector<Id> tokenIds;

	bool isEdge;
	bool isBundle;
	bool isFromSystem;
};

#endif // MESSAGE_ACTIVATE_TOKENS_H
