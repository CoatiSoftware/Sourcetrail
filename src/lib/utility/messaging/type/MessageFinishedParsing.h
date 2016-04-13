#ifndef MESSAGE_FINISHED_PARSING_H
#define MESSAGE_FINISHED_PARSING_H

#include <sstream>
#include <iomanip>

#include "data/ErrorCountInfo.h"
#include "utility/messaging/Message.h"
#include "utility/messaging/type/MessageStatus.h"

class MessageFinishedParsing
	: public Message<MessageFinishedParsing>
{
public:
	MessageFinishedParsing(size_t fileCount, size_t totalFileCount, float parseTime, bool loadedOnly = false)
		: fileCount(fileCount)
		, totalFileCount(totalFileCount)
		, parseTime(parseTime)
		, loadedOnly(loadedOnly)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageFinishedParsing";
	}

	virtual void dispatch()
	{
		Message<MessageFinishedParsing>::dispatch();
	}

	std::string getStatusStr() const
	{
		if (loadedOnly)
		{
			return "Finished loading";
		}

		std::stringstream ss;
		ss << "Finished analysis: ";
		ss << fileCount << "/" << totalFileCount << " files; ";

		float secondsLeft = parseTime;
		int hours = int(secondsLeft / 3600);
		secondsLeft -= hours * 3600;
		int minutes = int(secondsLeft / 60);
		secondsLeft -= minutes * 60;
		int seconds = int(secondsLeft);
		secondsLeft -= seconds;
		int milliSeconds = secondsLeft * 1000;

		if (hours > 9)
		{
			ss << hours;
		}
		else
		{
			ss << std::setw(2) << std::setfill('0') << hours;
		}
		ss << ":" << std::setw(2) << std::setfill('0') << minutes;
		ss << ":" << std::setw(2) << std::setfill('0') << seconds;

		if (!hours && !minutes)
		{
			ss << ":" << std::setw(3) << std::setfill('0') << milliSeconds;
		}

		return ss.str();
	}

	virtual void print(std::ostream& os) const
	{
		os << getStatusStr();
	}

	size_t fileCount;
	size_t totalFileCount;
	float parseTime;
	bool loadedOnly;
};

#endif // MESSAGE_FINISHED_PARSING_H
