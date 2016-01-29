#ifndef MESSAGE_SCROLL_CODE_H
#define MESSAGE_SCROLL_CODE_H

#include "utility/messaging/Message.h"

class MessageScrollCode
	: public Message<MessageScrollCode>
{
public:
	MessageScrollCode(int value)
		: value(value)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageScrollCode";
	}

	int value;
};

#endif // MESSAGE_SCROLL_CODE_H
