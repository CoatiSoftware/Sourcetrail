#ifndef MESSAGE_WINDOW_CHANGED_H
#define MESSAGE_WINDOW_CHANGED_H

#include "Message.h"

class MessageWindowChanged: public Message<MessageWindowChanged>
{
public:
	static const std::string getStaticType()
	{
		return "MessageWindowChanged";
	}
};

#endif	  // MESSAGE_WINDOW_CHANGED_H
