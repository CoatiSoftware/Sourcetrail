#ifndef MESSAGE_DISPLAY_BOOKMARKS_H
#define MESSAGE_DISPLAY_BOOKMARKS_H

#include "Bookmark.h"
#include "Message.h"

class MessageDisplayBookmarks
	: public Message<MessageDisplayBookmarks>
{
public:
	MessageDisplayBookmarks(
		Bookmark::BookmarkFilter filter = Bookmark::FILTER_UNKNOWN,
		Bookmark::BookmarkOrder order = Bookmark::ORDER_NONE
	)
		: filter(filter)
		, order(order)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageDisplayBookmarks";
	}

	const Bookmark::BookmarkFilter filter;
	const Bookmark::BookmarkOrder order;
};

#endif // MESSAGE_DISPLAY_BOOKMARKS_H