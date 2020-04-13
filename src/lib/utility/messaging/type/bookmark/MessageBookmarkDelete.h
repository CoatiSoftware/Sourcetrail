#ifndef MESSAGE_BOOKMARK_DELETE_H
#define MESSAGE_BOOKMARK_DELETE_H

#include "../../Message.h"

class MessageBookmarkDelete: public Message<MessageBookmarkDelete>
{
public:
	static const std::string getStaticType()
	{
		return "MessageBookmarkDelete";
	}
};

#endif	  // MESSAGE_BOOKMARK_DELETE_H
