#ifndef MESSAGE_UNDO_H
#define MESSAGE_UNDO_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageUndo: public Message<MessageUndo>
{
public:
	MessageUndo()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageUndo";
	}
};

#endif // MESSAGE_UNDO_H
