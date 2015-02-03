#ifndef MESSAGE_REDO_H
#define MESSAGE_REDO_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageRedo: public Message<MessageRedo>
{
public:
	MessageRedo()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageRedo";
	}
};

#endif // MESSAGE_REDO_H
