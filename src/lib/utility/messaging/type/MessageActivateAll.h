#ifndef MESSAGE_ACTIVATE_ALL_H
#define MESSAGE_ACTIVATE_ALL_H

#include "utility/messaging/Message.h"

class MessageActivateAll
	: public Message<MessageActivateAll>
{
public:
	MessageActivateAll()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageActivateAll";
	}
};

#endif // MESSAGE_ACTIVATE_ALL_H
