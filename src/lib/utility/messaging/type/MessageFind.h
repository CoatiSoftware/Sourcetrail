#ifndef MESSAGE_FIND_H
#define MESSAGE_FIND_H

#include "utility/messaging/Message.h"

class MessageFind
	: public Message<MessageFind>
{
public:
	MessageFind(bool fulltext = false)
		: findFulltext(fulltext)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageFind";
	}

	bool findFulltext;
};

#endif // MESSAGE_FIND_H
