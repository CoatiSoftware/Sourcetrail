#include "utility/logging/ConsoleLogger.h"

#include <iostream>

ConsoleLogger::ConsoleLogger()
	: Logger("ConsoleLogger")
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
	std::lock_guard<std::mutex> lock(m_consoleMutex);

	std::cout << message.getTimeString("%H:%M:%S") << " | ";

	if (!message.filePath.empty())
	{
		std::cout << message.getFileName() << ':' << message.line << ' ' << message.functionName << "() | ";
	}

	std::cout << type << ": ";
	std::wcout << message.message << std::endl;
}
