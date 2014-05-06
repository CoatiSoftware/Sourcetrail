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
	m_logManagerImplementation.addLogger(logger);
}

void LogManager::removeLogger(std::shared_ptr<Logger> logger)
{
	m_logManagerImplementation.removeLogger(logger);
}

void LogManager::removeLoggersByType(const std::string& type)
{
	m_logManagerImplementation.removeLoggersByType(type);
}

int LogManager::getLoggerCount() const
{
	return m_logManagerImplementation.getLoggerCount();
}

void LogManager::logInfo(
	const std::string& message,
	const std::string& file,
	const std::string& function,
	const unsigned int line
)
{
	m_logManagerImplementation.logInfo(message, file, function, line);
}

void LogManager::logWarning(
	const std::string& message,
	const std::string& file,
	const std::string& function,
	const unsigned int line
)
{
	m_logManagerImplementation.logWarning(message, file, function, line);
}

void LogManager::logError(
	const std::string& message,
	const std::string& file,
	const std::string& function,
	const unsigned int line
)
{
	m_logManagerImplementation.logError(message, file, function, line);
}

std::shared_ptr<LogManager> LogManager::s_instance;
std::mutex LogManager::s_instanceMutex;

LogManager::LogManager()
{
}
