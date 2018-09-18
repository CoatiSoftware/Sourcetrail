#ifndef MESSAGE_ERRORS_ALL_H
#define MESSAGE_ERRORS_ALL_H

#include "Message.h"

class MessageErrorsAll:
	public Message<MessageErrorsAll>
{
public:
	static const std::string getStaticType()
	{
		return "MessageErrorsAll";
	}

	MessageErrorsAll()
	{
	}
};

#endif // MESSAGE_ERRORS_ALL_H
