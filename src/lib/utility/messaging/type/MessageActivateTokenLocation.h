#ifndef MESSAGE_ACTIVATE_TOKEN_LOCATION_H
#define MESSAGE_ACTIVATE_TOKEN_LOCATION_H

#include "utility/messaging/Message.h"

class MessageActivateTokenLocation: public Message<MessageActivateTokenLocation>
{
public:
	static const std::string getStaticType()
	{
		return "MessageActivateTokenLocation";
	}
};

#endif // MESSAGE_ACTIVATE_TOKEN_LOCATION_H
