#include "component/controller/LogController.h"

#include "data/access/StorageAccess.h"
#include "utility/logging/LogManager.h"
#include "settings/ApplicationSettings.h"

LogController::LogController()
	: Logger("WindowLogger")
	, m_enabled(false)
	, m_previousLogCount(0)
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

void LogController::addLog(Logger::LogLevel type, const LogMessage& message)
{

	m_logs.push_back(
		Log(
			type,
			(message.getFileName().empty() ? "" : message.getFileName() + ": ") + message.message,
			message.getTimeString("%H:%M:%S")
		)
	);

	if (m_waiting.try_lock())
	{
		std::thread([&]()
			{
				std::this_thread::sleep_for( std::chrono::seconds(1) );
				syncLogs();
				m_waiting.unlock();
			}).detach();
	}
}

void LogController::syncLogs()
{
	int logCount = m_logs.size();
	if (logCount > getView()->LogLimit)
	{
		m_logs.erase(m_logs.begin(),m_logs.begin() + logCount - LogView::LogLimit);
	}
	std::vector<Log> logs;
	for( Log log : m_logs )
	{
		if (log.type & m_logLevel)
		{
			logs.push_back(log);
		}
	}

	getView()->addLogs(logs);
}

