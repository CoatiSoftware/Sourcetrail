#ifndef MESSAGE_FINISHED_PARSING_H
#define MESSAGE_FINISHED_PARSING_H

#include "utility/messaging/Message.h"

class MessageFinishedParsing: public Message<MessageFinishedParsing>
{
public:
	MessageFinishedParsing(float parseTime, size_t errorCount)
		: parseTime(parseTime)
		, errorCount(errorCount)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageFinishedParsing";
	}

	float parseTime;
	size_t errorCount;
};

#endif // MESSAGE_FINISHED_PARSING_H
