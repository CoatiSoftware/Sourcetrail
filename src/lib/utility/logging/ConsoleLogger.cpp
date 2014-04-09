#include "utility/logging/ConsoleLogger.h"

#include <iostream>

ConsoleLogger::ConsoleLogger()
	: Logger("ConsoleLogger")
{
}

ConsoleLogger::~ConsoleLogger()
{
}

void ConsoleLogger::logInfo(const LogMessage& message)
{
	logMessage("INFO", message);
}

void ConsoleLogger::logWarning(const LogMessage& message)
{
	logMessage("WARNING", message);
}

void ConsoleLogger::logError(const LogMessage& message)
{
	logMessage("ERROR", message);
}

void ConsoleLogger::logMessage(const std::string& type, const LogMessage& message)
{
	std::cout
		<< message.getTimeString("%H:%M:%S") << " | "
		<< message.getFileName() << ':' << message.line << ' ' << message.functionName << "() | "
		<< type << ": " << message.message
		<< std::endl;
}
