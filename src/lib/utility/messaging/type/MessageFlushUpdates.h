#ifndef MESSAGE_FLUSH_UPDATES_H
#define MESSAGE_FLUSH_UPDATES_H

#include "Message.h"

class MessageFlushUpdates:
	public Message<MessageFlushUpdates>
{
public:
	MessageFlushUpdates(bool keepsContent = false)
	{
		setKeepContent(keepsContent);
	}

	static const std::string getStaticType()
	{
		return "MessageFlushUpdates";
	}
};

#endif // MESSAGE_FLUSH_UPDATES_H
