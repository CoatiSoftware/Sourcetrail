#ifndef MESSAGE_DISPLAY_BOOKMARK_EDITOR_H
#define MESSAGE_DISPLAY_BOOKMARK_EDITOR_H

#include "data/bookmark/Bookmark.h"

#include "utility/messaging/Message.h"

class MessageDisplayBookmarkEditor
	: public Message<MessageDisplayBookmarkEditor>
{
public:
	MessageDisplayBookmarkEditor(std::shared_ptr<Bookmark> bookmark)
		: bookmark(bookmark)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageDisplayBookmarkEditor";
	}

	std::shared_ptr<Bookmark> bookmark;
};

#endif // MESSAGE_DISPLAY_BOOKMARK_EDITOR_H