#ifndef LOG_CONTROLLER_H
#define LOG_CONTROLLER_H

#include "component/controller/Controller.h"
#include "component/view/LogView.h"

#include "utility/logging/Logger.h"
#include "utility/logging/LogMessage.h"

class StorageAccess;

class LogController
	: public Controller
	, public Logger
{
public:
	LogController();
	~LogController();

private:
	LogView* getView() const;

	virtual void clear();

	virtual void logInfo(const LogMessage& message);
	virtual void logWarning(const LogMessage& message);
	virtual void logError(const LogMessage& message);

	void addLog(Logger::LogLevel type, const LogMessage& message);
};

#endif // LOG_CONTROLLER_H
