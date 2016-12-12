#ifndef MESSAGE_SHOW_STATUS_H
#define MESSAGE_SHOW_STATUS_H

#include "utility/messaging/Message.h"

class MessageShowStatus
	: public Message<MessageShowStatus>
{
public:
	MessageShowStatus()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageShowStatus";
	}
};

#endif // MESSAGE_SHOW_STATUS_H
