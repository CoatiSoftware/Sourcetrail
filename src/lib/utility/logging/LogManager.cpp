#include "LogManager.h"

#include <algorithm>

#include "logging.h"
#include "MessageStatus.h"
#include "utility.h"
#include "Version.h"
#include "utilityString.h"

std::shared_ptr<LogManager> LogManager::getInstance()
{
	if (s_instance.use_count() == 0)
	{
		s_instance = std::shared_ptr<LogManager>(new LogManager());
	}
	return s_instance;
}

void LogManager::destroyInstance()
{
	s_instance.reset();
}

LogManager::~LogManager()
{
}

void LogManager::setLoggingEnabled(bool enabled)
{
	if (m_loggingEnabled != enabled)
	{
		m_loggingEnabled = enabled;

		if (enabled)
		{
			LOG_INFO(
				std::string("Enabled logging for Sourcetrail ") +
				(utility::getApplicationArchitectureType() == APPLICATION_ARCHITECTURE_X86_32 ? "32" : "64") + " bit, " +
				"version " + Version::getApplicationVersion().toDisplayString()
			);
			MessageStatus(L"Enabled console and file logging.").dispatch();
		}
		else
		{
			LOG_INFO("Disabled logging");
			MessageStatus(L"Disabled console and file logging.").dispatch();
		}
	}
}

bool LogManager::getLoggingEnabled() const
{
	return m_loggingEnabled;
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

Logger* LogManager::getLogger(std::shared_ptr<Logger> logger)
{
	return m_logManagerImplementation.getLogger(logger);
}

Logger* LogManager::getLoggerByType(const std::string& type)
{
	return m_logManagerImplementation.getLoggerByType(type);
}

void LogManager::clearLoggers()
{
	m_logManagerImplementation.clearLoggers();
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
	if (m_loggingEnabled)
	{
		m_logManagerImplementation.logInfo(utility::decodeFromUtf8(message), file, function, line);
	}
}

void LogManager::logInfo(
	const std::wstring& message,
	const std::string& file,
	const std::string& function,
	const unsigned int line
)
{
	if (m_loggingEnabled)
	{
		m_logManagerImplementation.logInfo(message, file, function, line);
	}
}

void LogManager::logWarning(
	const std::string& message,
	const std::string& file,
	const std::string& function,
	const unsigned int line
)
{
	if (m_loggingEnabled)
	{
		m_logManagerImplementation.logWarning(utility::decodeFromUtf8(message), file, function, line);
	}
}

void LogManager::logWarning(
	const std::wstring& message,
	const std::string& file,
	const std::string& function,
	const unsigned int line
)
{
	if (m_loggingEnabled)
	{
		m_logManagerImplementation.logWarning(message, file, function, line);
	}
}

void LogManager::logError(
	const std::string& message,
	const std::string& file,
	const std::string& function,
	const unsigned int line
)
{
	if (m_loggingEnabled)
	{
		m_logManagerImplementation.logError(utility::decodeFromUtf8(message), file, function, line);
	}
}

void LogManager::logError(
	const std::wstring& message,
	const std::string& file,
	const std::string& function,
	const unsigned int line
)
{
	if (m_loggingEnabled)
	{
		m_logManagerImplementation.logError(message, file, function, line);
	}
}

std::shared_ptr<LogManager> LogManager::s_instance;

LogManager::LogManager()
	: m_loggingEnabled(false)
{
}
