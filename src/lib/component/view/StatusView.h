#ifndef STATUS_VIEW_H
#define STATUS_VIEW_H

#include "component/view/View.h"

#include "utility/Status.h"
#include "utility/logging/LogMessage.h"
#include "utility/logging/Logger.h"


class StatusView
	: public View
{
public:
	StatusView(ViewLayout* viewLayout);
	virtual ~StatusView();


	virtual std::string getName() const;
	virtual void addStatus(const std::vector<Status>& status) = 0;
	virtual void clear() = 0;
	//virtual bool hasLogLevel(const Logger::LogLevel type, const Logger::LogLevelMask mask) const;
	//virtual void addLog(Logger::LogLevel type, const LogMessage& message) = 0;
	//virtual void addLogs(const std::vector<Log>& logs) = 0;
	//static const int LogLimit;
};

#endif // STATUS_VIEW_H
