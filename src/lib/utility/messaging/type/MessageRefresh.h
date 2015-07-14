#ifndef MESSAGE_REFRESH_H
#define MESSAGE_REFRESH_H

#include "utility/messaging/Message.h"

class MessageRefresh: public Message<MessageRefresh>
{
public:
	MessageRefresh(bool uiOnly = false)
		: uiOnly(uiOnly)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageRefresh";
	}

	const bool uiOnly;
};

#endif // MESSAGE_REFRESH_H
