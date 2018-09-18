#ifndef MESSAGE_CLEAR_LOG_VIEW_H
#define MESSAGE_CLEAR_LOG_VIEW_H

#include "Message.h"

class MessageClearLogView:
	public Message<MessageClearLogView>
{
public:
	MessageClearLogView()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageClearLogView";
	}
};

#endif // MESSAGE_CLEAR_LOG_VIEW_H
