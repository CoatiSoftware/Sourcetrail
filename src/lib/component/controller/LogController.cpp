#include "component/controller/LogController.h"

#include "settings/ApplicationSettings.h"

LogController::LogController()
	: Logger("WindowLogger")
	, m_enabled(false)
{
}

LogController::~LogController()
{
}

void LogController::setEnabled(bool enabled)
{
	m_enabled = enabled;
}

bool LogController::getEnabled() const
{
	return m_enabled;
}

LogView* LogController::getView() const
{
	return Controller::getView<LogView>();
}

void LogController::clear()
{
	std::lock_guard<std::mutex> lock(m_logsMutex);
	m_logs.clear();
	getView()->clear();
}

void LogController::logInfo(const LogMessage& message )
{
	if (!m_enabled)
	{
		return;
	}
	if (m_logLevel & Logger::LOG_INFOS)
	{
		addLog(LOG_INFOS, message);
	}
}

void LogController::logError(const LogMessage& message )
{
	if (!m_enabled)
	{
		return;
	}
	if (m_logLevel & Logger::LOG_ERRORS)
	{
		addLog(LOG_ERRORS, message);
	}
}

void LogController::logWarning(const LogMessage& message )
{
	if (!m_enabled)
	{
		return;
	}
	if (m_logLevel & Logger::LOG_WARNINGS)
	{
		addLog(LOG_WARNINGS, message);
	}
}

void LogController::handleMessage(MessageLogFilterChanged* message)
{
	m_logLevel = message->logFilter;
	ApplicationSettings* settings = ApplicationSettings::getInstance().get();
	settings->setLogFilter(m_logLevel);
	settings->save();
	syncLogs();
}

void LogController::handleMessage(MessageClearLogView* message)
{
	clear();
}

void LogController::addLog(Logger::LogLevel type, const LogMessage& message)
{
	std::lock_guard<std::mutex> lock(m_logsMutex);
	m_logs.push_back(
		Log(
			type,
			(message.getFileName().empty() ? "" : message.getFileName() + ": ") + message.message,
			message.getTimeString("%H:%M:%S")
		)
	);

	if (!m_waiting)
	{
		m_waiting = true;
		std::thread([&]()
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				syncLogs();
				m_waiting = false;
			}
		).detach();
	}
}

void LogController::syncLogs()
{
	std::lock_guard<std::mutex> lock(m_logsMutex);

	int logCount = m_logs.size();
	if (logCount > getView()->LogLimit)
	{
		m_logs.erase(m_logs.begin(), m_logs.begin() + logCount - LogView::LogLimit);
	}

	std::vector<Log> logs;
	for (const Log& log : m_logs)
	{
		if (log.type & m_logLevel)
		{
			logs.push_back(log);
		}
	}

	getView()->addLogs(logs);
}

