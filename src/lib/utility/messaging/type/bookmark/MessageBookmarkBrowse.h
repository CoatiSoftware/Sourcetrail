#ifndef MESSAGE_BOOKMARKS_BROWSE_H
#define MESSAGE_BOOKMARKS_BROWSE_H

#include "../../../../data/bookmark/Bookmark.h"
#include "../../Message.h"

class MessageBookmarkBrowse: public Message<MessageBookmarkBrowse>
{
public:
	MessageBookmarkBrowse(
		Bookmark::BookmarkFilter filter = Bookmark::FILTER_UNKNOWN,
		Bookmark::BookmarkOrder order = Bookmark::ORDER_NONE)
		: filter(filter), order(order)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageBookmarkBrowse";
	}

	const Bookmark::BookmarkFilter filter;
	const Bookmark::BookmarkOrder order;
};

#endif	  // MESSAGE_BOOKMARKS_BROWSE_H
