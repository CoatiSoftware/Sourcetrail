#ifndef MESSAGE_TAB_CLOSE_H
#define MESSAGE_TAB_CLOSE_H

#include "Message.h"

class MessageTabClose: public Message<MessageTabClose>
{
public:
	static const std::string getStaticType()
	{
		return "MessageTabClose";
	}
};

#endif	  // MESSAGE_TAB_CLOSE_H
