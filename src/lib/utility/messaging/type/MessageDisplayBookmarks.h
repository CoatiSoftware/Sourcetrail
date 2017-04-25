#ifndef MESSAGE_DISPLAY_BOOKMARKS_H
#define MESSAGE_DISPLAY_BOOKMARKS_H

#include "utility/messaging/Message.h"

class MessageDisplayBookmarks
	: public Message<MessageDisplayBookmarks>
{
public:
	enum BookmarkFilter
	{
		UNKNOWN = 0,
		ALL,
		NODES,
		EDGES
	};

	enum BookmarkOrder
	{
		NONE = 0,
		DATE_ASCENDING,
		DATE_DESCENDING,
		NAME_ASCENDING,
		NAME_DESCENDING
	};

	MessageDisplayBookmarks(const BookmarkFilter& filter, const BookmarkOrder& order)
		: filter(filter)
		, order(order)
	{
	}

	MessageDisplayBookmarks()
		: filter(MessageDisplayBookmarks::BookmarkFilter::ALL)
		, order(MessageDisplayBookmarks::BookmarkOrder::NONE)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageDisplayBookmarks";
	}

	const BookmarkFilter filter;
	const BookmarkOrder order;
};

#endif // MESSAGE_DISPLAY_BOOKMARKS_H