#ifndef MESSAGE_DISPLAY_BOOKMARK_CREATOR_H
#define MESSAGE_DISPLAY_BOOKMARK_CREATOR_H

#include "utility/messaging/Message.h"

class MessageDisplayBookmarkCreator
	: public Message<MessageDisplayBookmarkCreator>
{
public:
	MessageDisplayBookmarkCreator()
		: nodeId(0)
	{
	}

	MessageDisplayBookmarkCreator(Id nodeId)
		: nodeId(nodeId)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageDisplayBookmarkCreator";
	}

	Id nodeId;
};

#endif // MESSAGE_DISPLAY_BOOKMARK_CREATOR_H