#include "utility/logging/LogManager.h"

#include <algorithm>

#include "utility/logging/LogMessage.h"

std::shared_ptr<LogManager> LogManager::getInstance()
{
	std::lock_guard<std::mutex> lockGuard(s_instanceMutex);
	if (s_instance.use_count() == 0)
	{
		s_instance = std::shared_ptr<LogManager>(new LogManager());
	}
	return s_instance;
}

void LogManager::destroyInstance()
{
	std::lock_guard<std::mutex> lockGuard(s_instanceMutex);
	s_instance.reset();
}

LogManager::~LogManager()
{
}

void LogManager::addLogger(std::shared_ptr<Logger> logger)
{
	std::lock_guard<std::mutex> lockGuard(m_loggerMutex);
	m_loggers.push_back(logger);
}

void LogManager::removeLogger(std::shared_ptr<Logger> logger)
{
	std::lock_guard<std::mutex> lockGuard(m_loggerMutex);
	std::vector<std::shared_ptr<Logger>>::iterator it = std::find(m_loggers.begin(), m_loggers.end(), logger);
	if (it != m_loggers.end())
	{
		m_loggers.erase(it);
	}
}

void LogManager::removeLoggersByType(const std::string& type)
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

int LogManager::getLoggerCount() const
{
	std::lock_guard<std::mutex> lockGuard(m_loggerMutex);
	return m_loggers.size();
}

void LogManager::logInfo(
	const std::string& message,
	const std::string& file,
	const std::string& function,
	const unsigned int line
)
{
	std::lock_guard<std::mutex> lockGuardLogger(m_loggerMutex);
	for (unsigned int i = 0; i < m_loggers.size(); i++)
	{
		m_loggers[i]->logInfo(LogMessage(message, file, function, line, getTime()));
	}
}

void LogManager::logWarning(
	const std::string& message,
	const std::string& file,
	const std::string& function,
	const unsigned int line
)
{
	std::lock_guard<std::mutex> lockGuardLogger(m_loggerMutex);
	for (unsigned int i = 0; i < m_loggers.size(); i++)
	{
		m_loggers[i]->logWarning(LogMessage(message, file, function, line, getTime()));
	}
}

void LogManager::logError(
	const std::string& message,
	const std::string& file,
	const std::string& function,
	const unsigned int line
)
{
	std::lock_guard<std::mutex> lockGuardLogger(m_loggerMutex);
	for (unsigned int i = 0; i < m_loggers.size(); i++)
	{
		m_loggers[i]->logError(LogMessage(message, file, function, line, getTime()));
	}
}

std::shared_ptr<LogManager> LogManager::s_instance;
std::mutex LogManager::s_instanceMutex;

LogManager::LogManager()
{
}

tm LogManager::getTime()
{
	time_t time;
	std::time(&time);
	tm result;
	result = *localtime(&time);
	return result;
}
