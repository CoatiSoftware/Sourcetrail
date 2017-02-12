#ifndef MESSAGE_DISPLAY_BOOKMARK_CREATOR_H
#define MESSAGE_DISPLAY_BOOKMARK_CREATOR_H

#include "utility/messaging/Message.h"

class MessageDisplayBookmarkCreator
	: public Message<MessageDisplayBookmarkCreator>
{
public:
	MessageDisplayBookmarkCreator()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageDisplayBookmarkCreator";
	}
};

#endif // MESSAGE_DISPLAY_BOOKMARK_CREATOR_H