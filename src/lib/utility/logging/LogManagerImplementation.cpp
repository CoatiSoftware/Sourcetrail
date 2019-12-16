#include "LogManagerImplementation.h"

#include <algorithm>

LogManagerImplementation::LogManagerImplementation() {}

LogManagerImplementation::LogManagerImplementation(const LogManagerImplementation& other)
{
	m_loggers = other.m_loggers;
}

void LogManagerImplementation::operator=(const LogManagerImplementation& other)
{
	m_loggers = other.m_loggers;
}

LogManagerImplementation::~LogManagerImplementation() {}

void LogManagerImplementation::addLogger(std::shared_ptr<Logger> logger)
{
	std::lock_guard<std::mutex> lockGuard(m_loggerMutex);
	m_loggers.push_back(logger);
}

void LogManagerImplementation::removeLogger(std::shared_ptr<Logger> logger)
{
	std::lock_guard<std::mutex> lockGuard(m_loggerMutex);
	std::vector<std::shared_ptr<Logger>>::iterator it = std::find(
		m_loggers.begin(), m_loggers.end(), logger);
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

Logger* LogManagerImplementation::getLogger(std::shared_ptr<Logger> logger)
{
	std::lock_guard<std::mutex> lockGuard(m_loggerMutex);
	std::vector<std::shared_ptr<Logger>>::iterator it = std::find(
		m_loggers.begin(), m_loggers.end(), logger);
	if (it != m_loggers.end())
	{
		return (*it).get();
	}
	return nullptr;
}

Logger* LogManagerImplementation::getLoggerByType(const std::string& type)
{
	std::lock_guard<std::mutex> lockGuard(m_loggerMutex);
	for (unsigned int i = 0; i < m_loggers.size(); i++)
	{
		if (m_loggers[i]->getType() == type)
		{
			return m_loggers[i].get();
		}
	}
	return nullptr;
}

void LogManagerImplementation::clearLoggers()
{
	m_loggers.clear();
}

int LogManagerImplementation::getLoggerCount() const
{
	std::lock_guard<std::mutex> lockGuard(m_loggerMutex);
	return static_cast<int>(m_loggers.size());
}

void LogManagerImplementation::logInfo(
	const std::wstring& message,
	const std::string& file,
	const std::string& function,
	const unsigned int line)
{
	std::lock_guard<std::mutex> lockGuardLogger(m_loggerMutex);
	for (unsigned int i = 0; i < m_loggers.size(); i++)
	{
		m_loggers[i]->onInfo(
			LogMessage(message, file, function, line, getTime(), std::this_thread::get_id()));
	}
}

void LogManagerImplementation::logWarning(
	const std::wstring& message,
	const std::string& file,
	const std::string& function,
	const unsigned int line)
{
	std::lock_guard<std::mutex> lockGuardLogger(m_loggerMutex);
	for (unsigned int i = 0; i < m_loggers.size(); i++)
	{
		m_loggers[i]->onWarning(
			LogMessage(message, file, function, line, getTime(), std::this_thread::get_id()));
	}
}

void LogManagerImplementation::logError(
	const std::wstring& message,
	const std::string& file,
	const std::string& function,
	const unsigned int line)
{
	std::lock_guard<std::mutex> lockGuardLogger(m_loggerMutex);
	for (unsigned int i = 0; i < m_loggers.size(); i++)
	{
		m_loggers[i]->onError(
			LogMessage(message, file, function, line, getTime(), std::this_thread::get_id()));
	}
}

tm LogManagerImplementation::getTime()
{
	time_t time;
	std::time(&time);

#pragma warning(push)
#pragma warning(disable : 4996)
	tm result = *std::localtime(&time);	   // this is done because localtime returns a pointer to a
										   // statically allocated object
#pragma warning(pop)

	return result;
}
