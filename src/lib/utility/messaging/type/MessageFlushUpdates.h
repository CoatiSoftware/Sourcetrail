#ifndef MESSAGE_FLUSH_UPDATES_H
#define MESSAGE_FLUSH_UPDATES_H

#include "utility/messaging/Message.h"

class MessageFlushUpdates:
	public Message<MessageFlushUpdates>
{
public:
	MessageFlushUpdates()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageFlushUpdates";
	}
};

#endif // MESSAGE_FLUSH_UPDATES_H
