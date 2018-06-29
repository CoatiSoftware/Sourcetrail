#ifndef MESSAGE_SHOW_ERROR_H
#define MESSAGE_SHOW_ERROR_H

#include "utility/messaging/Message.h"

class MessageShowError
	: public Message<MessageShowError>
{
public:
	static const std::string getStaticType()
	{
		return "MessageShowError";
	}

	MessageShowError(Id errorId)
		: errorId(errorId)
	{
	}

	const Id errorId;
};

#endif // MESSAGE_SHOW_ERROR_H
