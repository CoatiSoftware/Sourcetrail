#include "FileLogger.h"

#include <fstream>
#include <sstream>

const std::string FileLogger::s_filePath = "data/log/";

FileLogger::FileLogger()
	: Logger("FileLogger")
{
	setupFileName();
}

FileLogger::~FileLogger()
{
}

void FileLogger::logInfo(const LogMessage& message)
{
	logMessage("INFO", message);
}

void FileLogger::logWarning(const LogMessage& message)
{
	logMessage("WARNING", message);
}

void FileLogger::logError(const LogMessage& message)
{
	logMessage("ERROR", message);
}

void FileLogger::setupFileName()
{
	time_t time;
	std::time(&time);
	tm localTime = *std::localtime(&time);

	std::stringstream filename;
	filename << "log_";
	filename << localTime.tm_mon << "-" << localTime.tm_mday << "_";
	filename << localTime.tm_hour << "-" << localTime.tm_min << "-" << localTime.tm_sec << ".txt";

	m_fileName = filename.str();
}

void FileLogger::logMessage(const std::string& type, const LogMessage& message)
{
	std::ofstream fileStream;
	fileStream.open(s_filePath + m_fileName, std::ios::app);
	fileStream
		<< message.getTimeString("%H:%M:%S") << " | "
		<< message.getFileName() << ':' << message.line << ' ' << message.functionName << "() | "
		<< type << ": " << message.message
		<< std::endl;
	fileStream.close();
}
