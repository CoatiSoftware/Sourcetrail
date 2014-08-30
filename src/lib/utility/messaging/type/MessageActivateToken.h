#ifndef MESSAGE_ACTIVATE_TOKEN_H
#define MESSAGE_ACTIVATE_TOKEN_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageActivateToken: public Message<MessageActivateToken>
{
public:
	MessageActivateToken(Id tokenId)
		: tokenId(tokenId)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageActivateToken";
	}

	const Id tokenId;
};

#endif // MESSAGE_ACTIVATE_TOKEN_H
