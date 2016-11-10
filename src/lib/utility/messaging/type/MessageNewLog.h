#ifndef MESSAGE_NEW_LOG_H
#define MESSAGE_NEW_LOG_H

#include "utility/messaging/Message.h"

#include "utility/logging/logging.h"

class MessageNewLog
	: public Message<MessageNewLog>
{
public:
	MessageNewLog(const Logger::LogLevel type, const LogMessage& message)
		: type(type)
		, message(message)
	{
		setSendAsTask(false);
		setIsLogged(false);
	}

	static const std::string getStaticType()
	{
		return "MessageNewLog";
	}

	virtual void print(std::ostream& os) const
	{
		os << "Log: " << message.message;
	}

	const Logger::LogLevel type;
	const LogMessage message;
};

#endif // MESSAGE_NEW_LOG_H
