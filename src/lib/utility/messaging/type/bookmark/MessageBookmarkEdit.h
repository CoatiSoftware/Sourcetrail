#ifndef MESSAGE_BOOKMARK_EDIT_H
#define MESSAGE_BOOKMARK_EDIT_H

#include "Message.h"

class MessageBookmarkEdit: public Message<MessageBookmarkEdit>
{
public:
	static const std::string getStaticType()
	{
		return "MessageBookmarkEdit";
	}
};

#endif	  // MESSAGE_BOOKMARK_EDIT_H
