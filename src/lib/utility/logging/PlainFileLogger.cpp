#include "utility/logging/PlainFileLogger.h"

#include <fstream>
#include <sstream>

PlainFileLogger::PlainFileLogger(std::string filePath)
	: Logger("PlainFileLogger")
	, m_filePath(filePath)
{
	std::ofstream fileStream;
	fileStream.open(m_filePath, std::ios::trunc);
	fileStream.close();
}

PlainFileLogger::~PlainFileLogger()
{
}

void PlainFileLogger::logString(std::string str)
{
	std::ofstream fileStream;
	fileStream.open(m_filePath, std::ios::app);
	fileStream << str;
	fileStream.close();
}

void PlainFileLogger::logInfo(const LogMessage& message)
{
	logMessage("INFO", message);
}

void PlainFileLogger::logWarning(const LogMessage& message)
{
	logMessage("WARNING", message);
}

void PlainFileLogger::logError(const LogMessage& message)
{
	logMessage("ERROR", message);
}

void PlainFileLogger::logMessage(const std::string& type, const LogMessage& message)
{
	std::stringstream ss;
	ss << message.getFileName() << " " << type << ": " << message.message << std::endl;
	logString(ss.str());
}
