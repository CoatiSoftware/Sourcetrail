#ifndef MESSAGE_LOAD_AFTER_H
#define MESSAGE_LOAD_AFTER_H

#include "utility/messaging/Message.h"

class MessageLoadAfter
	: public Message<MessageLoadAfter>
{
public:
	MessageLoadAfter()
	{
		setSendAsTask(false);
	}

	static const std::string getStaticType()
	{
		return "MessageLoadAfter";
	}
};

#endif // MESSAGE_LOAD_AFTER_H
