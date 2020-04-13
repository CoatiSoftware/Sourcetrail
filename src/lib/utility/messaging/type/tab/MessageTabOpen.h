#ifndef MESSAGE_TAB_OPEN_H
#define MESSAGE_TAB_OPEN_H

#include "../../Message.h"

class MessageTabOpen: public Message<MessageTabOpen>
{
public:
	static const std::string getStaticType()
	{
		return "MessageTabOpen";
	}
};

#endif	  // MESSAGE_TAB_OPEN_H
