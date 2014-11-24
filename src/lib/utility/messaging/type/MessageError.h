#ifndef MESSAGE_ERROR_H
#define MESSAGE_ERROR_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageError: public Message<MessageError>
{
public:
	MessageError(
		const std::string& error
	)
		: error(error)

	{
	}

	static const std::string getStaticType()
	{
		return "MessageError";
	}

	const std::string error;
};

#endif // MESSAGE_ERROR_H
