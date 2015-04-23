#ifndef MESSAGE_FINISHED_PARSING_H
#define MESSAGE_FINISHED_PARSING_H

#include "utility/messaging/Message.h"

class MessageFinishedParsing: public Message<MessageFinishedParsing>
{
public:
	MessageFinishedParsing(size_t fileCount, size_t totalFileCount, float parseTime, size_t errorCount)
		: fileCount(fileCount)
		, totalFileCount(totalFileCount)
		, parseTime(parseTime)
		, errorCount(errorCount)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageFinishedParsing";
	}

	size_t fileCount;
	size_t totalFileCount;
	float parseTime;
	size_t errorCount;
};

#endif // MESSAGE_FINISHED_PARSING_H
