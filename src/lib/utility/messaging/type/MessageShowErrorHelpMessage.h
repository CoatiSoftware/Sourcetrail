#ifndef MESSAGE_SHOW_ERROR_HELP_MESSAGE_H
#define MESSAGE_SHOW_ERROR_HELP_MESSAGE_H

#include "utility/messaging/Message.h"

class MessageShowErrorHelpMessage:
	public Message<MessageShowErrorHelpMessage>
{
public:
	MessageShowErrorHelpMessage(bool force = false)
		: force(force)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageShowErrorHelpMessage";
	}

	const bool force;
};

#endif // MESSAGE_SHOW_ERROR_HELP_MESSAGE_H
