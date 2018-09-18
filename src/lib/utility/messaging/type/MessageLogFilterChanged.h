#ifndef MESSAGE_LOG_FILTER_CHANGED_H
#define MESSAGE_LOG_FILTER_CHANGED_H

#include "Message.h"
#include "Logger.h"

class MessageLogFilterChanged
	: public Message<MessageLogFilterChanged>
{
public:
	MessageLogFilterChanged(const Logger::LogLevelMask filter)
		: logFilter(filter)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageLogFilterChanged";
	}

	const Logger::LogLevelMask logFilter;
};

#endif // MESSAGE_LOG_FILTER_CHANGED_H
