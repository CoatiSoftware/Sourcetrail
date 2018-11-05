#ifndef MESSAGE_HISTORY_TO_POSITION_H
#define MESSAGE_HISTORY_TO_POSITION_H

#include "Message.h"
#include "TabId.h"

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
		setSchedulerId(TabId::currentTab());
	}

	const size_t index;
};

#endif // MESSAGE_HISTORY_TO_POSITION_H
