#ifndef MESSAGE_LOAD_BEFORE_H
#define MESSAGE_LOAD_BEFORE_H

#include "utility/messaging/Message.h"

class MessageLoadBefore
	: public Message<MessageLoadBefore>
{
public:
	MessageLoadBefore()
	{
		setSendAsTask(false);
	}

	static const std::string getStaticType()
	{
		return "MessageLoadBefore";
	}
};

#endif // MESSAGE_LOAD_BEFORE_H
