#ifndef MESSAGE_ERRORS_HELP_MESSAGE_H
#define MESSAGE_ERRORS_HELP_MESSAGE_H

#include "../../Message.h"

class MessageErrorsHelpMessage: public Message<MessageErrorsHelpMessage>
{
public:
	static const std::string getStaticType()
	{
		return "MessageErrorsHelpMessage";
	}

	MessageErrorsHelpMessage(bool force = false): force(force) {}

	const bool force;
};

#endif	  // MESSAGE_ERRORS_HELP_MESSAGE_H
