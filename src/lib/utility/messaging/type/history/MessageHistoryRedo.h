#ifndef MESSAGE_HISTORY_REDO_H
#define MESSAGE_HISTORY_REDO_H

#include "Message.h"

class MessageHistoryRedo
	: public Message<MessageHistoryRedo>
{
public:
	static const std::string getStaticType()
	{
		return "MessageHistoryRedo";
	}

	MessageHistoryRedo()
	{
	}
};

#endif // MESSAGE_HISTORY_REDO_H
