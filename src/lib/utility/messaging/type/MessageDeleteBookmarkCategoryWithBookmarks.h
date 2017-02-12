#ifndef MESSAGE_DELETE_BOOKMARK_CATEGORY_WITH_BOOKMARKS_H
#define MESSAGE_DELETE_BOOKMARK_CATEGORY_WITH_BOOKMARKS_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageDeleteBookmarkCategoryWithBookmarks
	: public Message<MessageDeleteBookmarkCategoryWithBookmarks>
{
public:
	MessageDeleteBookmarkCategoryWithBookmarks(const Id id)
		: categoryId(id)
	{
	}

	~MessageDeleteBookmarkCategoryWithBookmarks()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageDeleteBookmarkCategoryWithBookmarks";
	}

	const Id categoryId;
};

#endif // MESSAGE_DELETE_BOOKMARK_CATEGORY_WITH_BOOKMARKS_H