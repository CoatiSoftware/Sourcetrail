#ifndef MESSAGE_FINISHED_PARSING_H
#define MESSAGE_FINISHED_PARSING_H

#include <sstream>
#include <iomanip>

#include "utility/messaging/Message.h"
#include "utility/messaging/type/MessageStatus.h"

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

	virtual void dispatch()
	{
		MessageStatus(getStatusStr(), errorCount > 0).dispatch();

		Message<MessageFinishedParsing>::dispatch();
	}

	std::string getStatusStr() const
	{
		std::stringstream ss;
		ss << "Parsing Finished: ";
		ss << fileCount << "/" << totalFileCount << " files, ";
		ss << std::setprecision(2) << std::fixed << parseTime << " seconds, ";
		ss << errorCount << " error(s)";
		return ss.str();
	}

	size_t fileCount;
	size_t totalFileCount;
	float parseTime;
	size_t errorCount;
};

#endif // MESSAGE_FINISHED_PARSING_H
