#ifndef MESSAGE_FINISHED_PARSING_H
#define MESSAGE_FINISHED_PARSING_H

#include "utility/messaging/Message.h"

class MessageFinishedParsing: public Message<MessageFinishedParsing>
{
public:
	MessageFinishedParsing()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageFinishedParsing";
	}
};

#endif // MESSAGE_FINISHED_PARSING_H
