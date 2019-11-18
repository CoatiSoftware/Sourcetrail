#ifndef MESSAGE_ERROR_COUNT_CLEAR_H
#define MESSAGE_ERROR_COUNT_CLEAR_H

#include "Message.h"

class MessageErrorCountClear: public Message<MessageErrorCountClear>
{
public:
	static const std::string getStaticType()
	{
		return "MessageErrorCountClear";
	}

	MessageErrorCountClear()
	{
		setSendAsTask(false);
	}
};

#endif	  // MESSAGE_ERROR_COUNT_CLEAR_H
