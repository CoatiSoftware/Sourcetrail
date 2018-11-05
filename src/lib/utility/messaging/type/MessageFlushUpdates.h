#ifndef MESSAGE_FLUSH_UPDATES_H
#define MESSAGE_FLUSH_UPDATES_H

#include "Message.h"
#include "TabId.h"

class MessageFlushUpdates:
	public Message<MessageFlushUpdates>
{
public:
	MessageFlushUpdates(bool keepsContent = false)
	{
		setKeepContent(keepsContent);
		setSchedulerId(TabId::currentTab());
	}

	static const std::string getStaticType()
	{
		return "MessageFlushUpdates";
	}
};

#endif // MESSAGE_FLUSH_UPDATES_H
