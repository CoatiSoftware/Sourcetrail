#ifndef MESSAGE_CREATE_BOOKMARK_CATEGORY_H
#define MESSAGE_CREATE_BOOKMARK_CATEGORY_H

#include "utility/messaging/Message.h"

class MessageCreateBookmarkCategory
	: public Message<MessageCreateBookmarkCategory>
{
public:
	MessageCreateBookmarkCategory(const std::string& name)
		: name(name)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageCreateBookmarkCategory";
	}

	const std::string name;
};

#endif // MESSAGE_CREATE_BOOKMARK_CATEGORY_H