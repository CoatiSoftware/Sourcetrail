#ifndef PLAIN_FILE_LOGGER_H
#define PLAIN_FILE_LOGGER_H

#include <string>

#include "utility/logging/Logger.h"
#include "utility/logging/LogMessage.h"

class PlainFileLogger: public Logger
{
public:
	PlainFileLogger(std::string filePath);
	virtual ~PlainFileLogger();

	void logString(std::string str);

private:
	virtual void logInfo(const LogMessage& message);
	virtual void logWarning(const LogMessage& message);
	virtual void logError(const LogMessage& message);

	void logMessage(const std::string& type, const LogMessage& message);

	std::string m_filePath;
};

#endif // PLAIN_FILE_LOGGER_H
