#ifndef MESSAGE_DISPLAY_BOOKMARK_CREATOR_H
#define MESSAGE_DISPLAY_BOOKMARK_CREATOR_H

#include "Message.h"

class MessageDisplayBookmarkCreator
	: public Message<MessageDisplayBookmarkCreator>
{
public:
	MessageDisplayBookmarkCreator(Id nodeId = 0)
		: nodeId(nodeId)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageDisplayBookmarkCreator";
	}

	const Id nodeId;
};

#endif // MESSAGE_DISPLAY_BOOKMARK_CREATOR_H