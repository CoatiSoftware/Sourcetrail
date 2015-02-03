#ifndef MESSAGE_AUTO_REFRESH_CHANGED_H
#define MESSAGE_AUTO_REFRESH_CHANGED_H

#include "utility/messaging/Message.h"

class MessageAutoRefreshChanged: public Message<MessageAutoRefreshChanged>
{
public:
	MessageAutoRefreshChanged(bool enabled)
		: enabled(enabled)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageAutoRefreshChanged";
	}

	bool enabled;
};

#endif // MESSAGE_AUTO_REFRESH_CHANGED_H
