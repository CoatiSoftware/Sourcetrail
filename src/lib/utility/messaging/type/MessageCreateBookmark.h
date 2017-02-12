#ifndef MESSAGE_CREATE_BOOKMARK_H
#define MESSAGE_CREATE_BOOKMARK_H

#include "utility/messaging/Message.h"

class MessageCreateBookmark
	: public Message<MessageCreateBookmark>
{
public:
	MessageCreateBookmark(const std::string& comment, const std::string& displayName, const std::string& categoryName)
		: comment(comment)
		, displayName(displayName)
		, categoryName(categoryName)
	{
	}

	~MessageCreateBookmark()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageCreateBookmark";
	}

	const std::string comment;
	const std::string displayName;
	const std::string categoryName;
};

#endif // MESSAGE_CREATE_BOOKMARK_H