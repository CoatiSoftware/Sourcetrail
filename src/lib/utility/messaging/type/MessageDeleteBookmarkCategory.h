#ifndef MESSAGE_DELETE_BOOKMARK_CATEGORY_H
#define MESSAGE_DELETE_BOOKMARK_CATEGORY_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageDeleteBookmarkCategory
	: public Message<MessageDeleteBookmarkCategory>
{
public:
	MessageDeleteBookmarkCategory(const Id id)
		: categoryId(id)
	{
	}

	~MessageDeleteBookmarkCategory()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageDeleteBookmarkCategory";
	}

	const Id categoryId;
};

#endif // MESSAGE_DELETE_BOOKMARK_CATEGORY_H
