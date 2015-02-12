#include "LogManagerImplementation.h"

#include <algorithm>

LogManagerImplementation::LogManagerImplementation()
{
}

LogManagerImplementation::LogManagerImplementation(const LogManagerImplementation& other)
{
	m_loggers = other.m_loggers;
}

void LogManagerImplementation::operator=(const LogManagerImplementation& other)
{
	m_loggers = other.m_loggers;
}

LogManagerImplementation::~LogManagerImplementation()
{
}

void LogManagerImplementation::addLogger(std::shared_ptr<Logger> logger)
{
	std::lock_guard<std::mutex> lockGuard(m_loggerMutex);
	m_loggers.push_back(logger);
}

void LogManagerImplementation::removeLogger(std::shared_ptr<Logger> logger)
{
	std::lock_guard<std::mutex> lockGuard(m_loggerMutex);
	std::vector<std::shared_ptr<Logger>>::iterator it = std::find(m_loggers.begin(), m_loggers.end(), logger);
	if (it != m_loggers.end())
	{
		m_loggers.erase(it);
	}
}

void LogManagerImplementation::removeLoggersByType(const std::string& type)
{
	std::lock_guard<std::mutex> lockGuard(m_loggerMutex);
	for (unsigned int i = 0; i < m_loggers.size(); i++)
	{
		if (m_loggers[i]->getType() == type)
		{
			m_loggers.erase(m_loggers.begin() + i);
			i--;
		}
	}
}

int LogManagerImplementation::getLoggerCount() const
{
	std::lock_guard<std::mutex> lockGuard(m_loggerMutex);
	return m_loggers.size();
}

void LogManagerImplementation::logInfo(
	const std::string& message,
	const std::string& file,
	const std::string& function,
	const unsigned int line
)
{
	std::lock_guard<std::mutex> lockGuardLogger(m_loggerMutex);
	for (unsigned int i = 0; i < m_loggers.size(); i++)
	{
		m_loggers[i]->onInfo(LogMessage(message, file, function, line, getTime()));
	}
}

void LogManagerImplementation::logWarning(
	const std::string& message,
	const std::string& file,
	const std::string& function,
	const unsigned int line
)
{
	std::lock_guard<std::mutex> lockGuardLogger(m_loggerMutex);
	for (unsigned int i = 0; i < m_loggers.size(); i++)
	{
		m_loggers[i]->onWarning(LogMessage(message, file, function, line, getTime()));
	}
}

void LogManagerImplementation::logError(
	const std::string& message,
	const std::string& file,
	const std::string& function,
	const unsigned int line
)
{
	std::lock_guard<std::mutex> lockGuardLogger(m_loggerMutex);
	for (unsigned int i = 0; i < m_loggers.size(); i++)
	{
		m_loggers[i]->onError(LogMessage(message, file, function, line, getTime()));
	}
}

tm LogManagerImplementation::getTime()
{
	time_t time;
	std::time(&time);
	tm result = *std::localtime(&time); // this is done because localtime returns a pointer to a statically allocated object
	return result;
}
