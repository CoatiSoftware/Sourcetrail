#ifndef LOG_VIEW_H
#define LOG_VIEW_H

#include "component/view/View.h"

#include "utility/logging/LogMessage.h"
#include "utility/logging/Logger.h"

struct Log
{
	Log(Logger::LogLevel type, std::string message, std::string timestamp)
		: type(type)
		, message(message)
		, timestamp(timestamp){}
	Logger::LogLevel type;
	std::string message;
	std::string timestamp;
};

class LogView
	: public View
{
public:
	LogView(ViewLayout* viewLayout);
	virtual ~LogView();


	virtual std::string getName() const;
	virtual bool hasLogLevel(const Logger::LogLevel type, const Logger::LogLevelMask mask) const;

	virtual void clear() = 0;
	virtual void addLog(Logger::LogLevel type, const LogMessage& message) = 0;
	virtual void addLogs(const std::vector<Log>& logs) = 0;
	static const int LogLimit;
};

#endif // LOG_VIEW_H
