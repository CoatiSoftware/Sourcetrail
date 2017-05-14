#ifndef MESSAGE_CREATE_BOOKMARK_H
#define MESSAGE_CREATE_BOOKMARK_H

#include "utility/messaging/Message.h"

class MessageCreateBookmark
	: public Message<MessageCreateBookmark>
{
public:
	MessageCreateBookmark(const std::string& comment, const std::string& displayName, const std::string& categoryName, Id nodeId)
		: comment(comment)
		, displayName(displayName)
		, categoryName(categoryName)
		, nodeId(nodeId)
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
	const Id nodeId;
};

#endif // MESSAGE_CREATE_BOOKMARK_H