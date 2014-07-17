#ifndef MESSAGE_FIND_H
#define MESSAGE_FIND_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageFind: public Message<MessageFind>
{
public:
	MessageFind()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageFind";
	}
};

#endif // MESSAGE_FIND_H
