#ifndef MESSAGE_PING_RECEIVED_H
#define MESSAGE_PING_RECEIVED_H

#include "utility/messaging/Message.h"

class MessagePingReceived
	: public Message<MessagePingReceived>
{
public:
	MessagePingReceived()
		: ideId("")
	{
	}

	static const std::string getStaticType()
	{
		return "MessagePingReceived";
	}

	std::string ideId;
};

#endif // MESSAGE_PING_RECEIVED_H