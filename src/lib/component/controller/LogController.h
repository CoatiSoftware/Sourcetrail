#ifndef LOG_CONTROLLER_H
#define LOG_CONTROLLER_H

#include <mutex>

#include "component/controller/Controller.h"
#include "component/view/LogView.h"

#include "utility/logging/Logger.h"
#include "utility/logging/LogMessage.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageLogFilterChanged.h"

class StorageAccess;

class LogController
	: public Controller
	, public Logger
	, public MessageListener<MessageLogFilterChanged>
{
public:
	LogController();
	~LogController();

	void setEnabled(bool enabled);
	bool getEnabled() const;

private:
	bool m_enabled;
	LogView* getView() const;

	virtual void clear();

	virtual void logInfo(const LogMessage& message);
	virtual void logWarning(const LogMessage& message);
	virtual void logError(const LogMessage& message);

	virtual void handleMessage(MessageLogFilterChanged* message);

	std::vector<Log> m_logs;

	void addLog(Logger::LogLevel type, const LogMessage& message);
	void syncLogs();
	std::mutex m_waiting;
	int m_previousLogCount;
	Logger::LogLevelMask m_logLevel;
};

#endif // LOG_CONTROLLER_H
