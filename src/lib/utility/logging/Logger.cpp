#include "utility/logging/Logger.h"

Logger::Logger(const std::string& type)
	: m_type(type)
	, m_levelMask(LOG_ALL)
{
}

Logger::~Logger()
{
}

std::string Logger::getType() const
{
	return m_type;
}

Logger::LogLevelMask Logger::getLogLevel() const
{
	return m_levelMask;
}

void Logger::setLogLevel(LogLevelMask mask)
{
	m_levelMask = mask;
}

bool Logger::isLogLevel(LogLevelMask mask)
{
	return (m_levelMask & mask) > 0;
}

void Logger::onInfo(const LogMessage& message)
{
	if (isLogLevel(LOG_INFOS))
	{
		logInfo(message);
	}
}

void Logger::onWarning(const LogMessage& message)
{
	if (isLogLevel(LOG_WARNINGS))
	{
		logWarning(message);
	}
}

void Logger::onError(const LogMessage& message)
{
	if (isLogLevel(LOG_ERRORS))
	{
		logError(message);
	}
}
