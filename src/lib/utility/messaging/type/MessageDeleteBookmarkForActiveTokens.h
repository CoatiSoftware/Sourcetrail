#ifndef MESSAGE_DELETE_BOOKMARK_FOR_ACTIVE_TOKENS
#define MESSAGE_DELETE_BOOKMARK_FOR_ACTIVE_TOKENS

#include "utility/messaging/Message.h"

class MessageDeleteBookmarkForActiveTokens
	: public Message<MessageDeleteBookmarkForActiveTokens>
{
public:
	MessageDeleteBookmarkForActiveTokens()
	{
	}

	~MessageDeleteBookmarkForActiveTokens()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageDeleteBookmarkForActiveTokens";
	}
};

#endif // MESSAGE_DELETE_BOOKMARK_FOR_ACTIVE_TOKENS
