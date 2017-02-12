#ifndef MESSAGE_DELETE_BOOKMARK_H
#define MESSAGE_DELETE_BOOKMARK_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageDeleteBookmark
	: public Message<MessageDeleteBookmark>
{
public:
	MessageDeleteBookmark(const Id bookmarkId, const bool isEdge)
		: bookmarkId(bookmarkId)
		, isEdge(isEdge)
	{
	}

	~MessageDeleteBookmark()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageDeleteBookmark";
	}

	const Id bookmarkId;
	const bool isEdge;
};

#endif // MESSAGE_DELETE_BOOKMARK_H