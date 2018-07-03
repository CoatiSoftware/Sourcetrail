#ifndef MESSAGE_HISTORY_UNDO_H
#define MESSAGE_HISTORY_UNDO_H

#include "utility/messaging/Message.h"

class MessageHistoryUndo
	: public Message<MessageHistoryUndo>
{
public:
	static const std::string getStaticType()
	{
		return "MessageHistoryUndo";
	}

	MessageHistoryUndo()
	{
	}
};

#endif // MESSAGE_HISTORY_UNDO_H
