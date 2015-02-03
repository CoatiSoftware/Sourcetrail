#ifndef MESSAGE_REFRESH_H
#define MESSAGE_REFRESH_H

#include "utility/messaging/Message.h"

class MessageRefresh: public Message<MessageRefresh>
{
public:
	MessageRefresh()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageRefresh";
	}
};

#endif // MESSAGE_REFRESH_H
