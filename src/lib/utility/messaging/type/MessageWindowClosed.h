#ifndef MESSAGE_WINDOW_CLOSED_H
#define MESSAGE_WINDOW_CLOSED_H

#include "../Message.h"

class MessageWindowClosed: public Message<MessageWindowClosed>
{
public:
	MessageWindowClosed()
	{
		setSendAsTask(false);
	}

	static const std::string getStaticType()
	{
		return "MessageWindowClosed";
	}
};

#endif	  // MESSAGE_WINDOW_CLOSED_H
