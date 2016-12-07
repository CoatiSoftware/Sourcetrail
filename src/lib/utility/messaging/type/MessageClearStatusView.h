#ifndef MESSAGE_CLEAR_STATUS_VIEW_H
#define MESSAGE_CLEAR_STATUS_VIEW_H

#include "utility/messaging/Message.h"

class MessageClearStatusView:
	public Message<MessageClearStatusView>
{
public:
	MessageClearStatusView()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageClearStatusView";
	}
};

#endif // MESSAGE_CLEAR_STATUS_VIEW_H
