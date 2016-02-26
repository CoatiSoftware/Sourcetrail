#include "FileLogger.h"

#include <fstream>
#include <sstream>
#include <cstdio>

#include "utility/file/FileSystem.h"

std::string FileLogger::s_filePath = "user/log/";
unsigned int FileLogger::s_maxLogCount = 1000;
unsigned int FileLogger::s_amountOfLogFiles = 2;

FileLogger::FileLogger()
	: Logger("FileLogger")
	, m_logCount(0)
	, m_suffix(1)
{
	setupFileName();
	changeLogFile();
}

FileLogger::~FileLogger()
{
}

void FileLogger::setFilePath(const std::string& filePath)
{
	s_filePath = filePath;

	createDirectory();
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
	tm t = *std::localtime(&time);

	std::stringstream filename;
	filename << "log_";
	filename << t.tm_year + 1900 << "-";
	filename << (t.tm_mon < 9 ? "0" : "") << t.tm_mon + 1 << "-";
	filename << (t.tm_mday < 10 ? "0" : "") << t.tm_mday << "_";
	filename << (t.tm_hour < 10 ? "0" : "") << t.tm_hour << "-";
	filename << (t.tm_min < 10 ? "0" : "") << t.tm_min << "-";
	filename << (t.tm_sec < 10 ? "0" : "") << t.tm_sec << "_";

	m_fileName = filename.str();

}

void FileLogger::createDirectory()
{
	if (s_filePath.length() > 0)
	{
		FileSystem::createDirectory(s_filePath);
	}
}

void FileLogger::setMaxLogCount(unsigned int logCount)
{
	s_maxLogCount = logCount;
}

void FileLogger::changeLogFile()
{
	m_suffix = ++m_suffix % s_amountOfLogFiles;
	m_currentLogFile = m_fileName + (m_suffix ? "1" : "0") + ".txt";

	FileSystem::remove(s_filePath + m_currentLogFile);
}

void FileLogger::logMessage(const std::string& type, const LogMessage& message)
{
	std::ofstream fileStream;
	fileStream.open(s_filePath + m_currentLogFile, std::ios::app);
	fileStream << message.getTimeString("%H:%M:%S") << " | ";

	if (message.filePath.size())
	{
		fileStream << message.getFileName() << ':' << message.line << ' ' << message.functionName << "() | ";
	}

	fileStream << type << ": " << message.message << std::endl;
	fileStream.close();

	if (++m_logCount >= s_maxLogCount)
	{
		changeLogFile();
		m_logCount = 0;
	}
}
