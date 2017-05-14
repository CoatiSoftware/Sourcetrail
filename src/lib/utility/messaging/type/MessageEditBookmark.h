#ifndef MESSAGE_EDIT_BOOKMARK_H
#define MESSAGE_EDIT_BOOKMARK_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageEditBookmark
	: public Message<MessageEditBookmark>
{
public:
	MessageEditBookmark(Id id, const std::string& comment, const std::string& displayName, const std::string& categoryName)
		: bookmarkId(id)
		, comment(comment)
		, displayName(displayName)
		, categoryName(categoryName)
	{
	}

	~MessageEditBookmark()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageEditBookmark";
	}

	const Id bookmarkId;
	const std::string comment;
	const std::string displayName;
	const std::string categoryName;
};

#endif // MESSAGE_EDIT_BOOKMARK_H