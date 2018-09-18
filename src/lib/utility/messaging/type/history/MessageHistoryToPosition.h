#ifndef MESSAGE_HISTORY_TO_POSITION_H
#define MESSAGE_HISTORY_TO_POSITION_H

#include "Message.h"

class MessageHistoryToPosition
	: public Message<MessageHistoryToPosition>
{
public:
	static const std::string getStaticType()
	{
		return "MessageHistoryToPosition";
	}

	MessageHistoryToPosition(size_t index)
		: index(index)
	{
	}

	const size_t index;
};

#endif // MESSAGE_HISTORY_TO_POSITION_H
