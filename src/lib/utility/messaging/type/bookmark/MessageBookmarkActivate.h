#ifndef MESSAGE_BOOKMARK_ACTIVATE_H
#define MESSAGE_BOOKMARK_ACTIVATE_H

#include "../../../../data/bookmark/Bookmark.h"
#include "../../Message.h"

class MessageBookmarkActivate: public Message<MessageBookmarkActivate>
{
public:
	MessageBookmarkActivate(const std::shared_ptr<Bookmark>& bookmark): bookmark(bookmark) {}

	static const std::string getStaticType()
	{
		return "MessageBookmarkActivate";
	}

	const std::shared_ptr<Bookmark> bookmark;
};

#endif	  // MESSAGE_BOOKMARK_ACTIVATE_H
