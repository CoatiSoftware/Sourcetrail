#ifndef MESSAGE_CLEAR_ERROR_COUNT_H
#define MESSAGE_CLEAR_ERROR_COUNT_H

#include "utility/messaging/Message.h"

class MessageClearErrorCount
	: public Message<MessageClearErrorCount>
{
public:
	MessageClearErrorCount()
	{
		setSendAsTask(false);
	}

	static const std::string getStaticType()
	{
		return "MessageClearErrorCount";
	}
};

#endif // MESSAGE_CLEAR_ERROR_COUNT_H
