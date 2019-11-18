#ifndef CONSOLE_LOGGER_H
#define CONSOLE_LOGGER_H

#include <mutex>

#include "LogMessage.h"
#include "Logger.h"

class ConsoleLogger: public Logger
{
public:
	ConsoleLogger();

private:
	void logInfo(const LogMessage& message) override;
	void logWarning(const LogMessage& message) override;
	void logError(const LogMessage& message) override;

	void logMessage(const std::string& type, const LogMessage& message);
};

#endif	  // CONSOLE_LOGGER_H
